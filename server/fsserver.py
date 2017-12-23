from collections import deque
from enum import Enum
import sys
from twisted.logger import Logger, FileLogObserver, globalLogPublisher, formatEventAsClassicLogText
from twisted.internet import ssl, defer, reactor, protocol, task
from twisted.protocols import basic
from twisted.python.modules import getModule

def cleanInput(line):
    line = line.strip()
    line = line.split(' ', 1)

    return line

class FSProtocol(basic.LineReceiver):

    class State(Enum):
        IDLE    = 0
        POLLING = 1
        POLLED  = 2
        RAW     = 3

    def __init__(self):
        self.log = Logger()
        self.setIdleState()

    def setIdleState(self):
        self.my_state = self.State.IDLE
        self.wait_for_list = None
        self.file_transfer_peer = None
        self.setLineMode()

    def connectionMade(self):
        self.log.info("New connection established: {peer!r}, {pid!s}", 
                peer=self.transport.getPeer(), pid=id(self))
        self.factory.protocols.append(self)
        self.transport.write(b'Hello form server\r\n')

    def send(self, command, argument):
        if command != "":
            command += " ";
        self.transport.write(command.upper().encode() + argument.encode() + b'\r\n')
        #  self.log.info("Sending to {peer!r}: {command!s} {argument!s}", 
        #          command=command, argument=argument)

    def doReject(self, line):
        self.log.error("We had to say no to {peer!r}: {line}",
                peer=self.transport.getPeer(), line=line)
        self.transport.write(b'REJECT ' + line.encode() + b'\r\n')

    #  def doRequest(self, regex):
    #      self.log.debug("server asks for files: {regex!s}", regex=regex)
    #      self.transport.write(b'REQUEST ' + regex.encode() + b'\r\n')

    def requestFiles(self, regex):
        '''
        Called by factory, if one of users requested a search
        '''
        if self.my_state != self.State.IDLE or self.wait_for_list is not None:
            return defer.fail(RuntimeError("Connection is busy"))

        # Go to POLLED state, so no one will interrupt us
        self.my_state = self.State.POLLED

        self.send("REQUEST", regex)

        def debuglogc(data):
            self.log.debug("wait_for_list:requestFiles:callback: {data!r}", data=data)
            return data

        def debugloge(err):
            self.log.debug("wait_for_list:requestFiles:errback: {err!r}", err=err)
            return err

        self.wait_for_list = defer.Deferred().addCallbacks(debuglogc,debugloge)
        # We create Deferred, that will be fired as the response comes from user
        return self.wait_for_list

    def trySetStateIdle(self):
        # Some operation has finished
        # Check if there is file request
        #  if len(self.outer_req) > 0:
        #      # Set state
        #      self.state = self.State.POLLED
        #      # Ask peer
        #      regex,d = self.outer_req[0]
        #      self.transport.write("TODO some message" + regex + b'\r\n')
        pass

    def handleFind(self, regex):
        '''
        Hanles the FIND request from user
        '''
        self.my_state = self.State.POLLING

        # The factory will ask all the others about our regexp
        # The result here is the string
        d = self.factory.getFiles(self, regex)

        def response(ans):
            self.log.info("FIND command returns next {filelist!s}", 
                    filelist=ans)
            self.send('RESPONSE', ans)

        def error(e):
            self.doReject(e.getErrorMessage())

        def idle(ignore):
            self.my_state = self.State.IDLE

        d.addCallbacks(response, error)
        d.addBoth(idle)

    def lineReceived(self, line):
        self.log.info("lineReceived: {peer!r} got message: {line!s}",
                peer=self.transport.getPeer(), line=line)

        line = line.decode()
        data = cleanInput(line)

        if len(data) == 0 or data == '':
            self.log.info("Oh, never mind, it's gone")
            self.doReject("Seems like a bunch of tabs")
            return
        
        command = data[0].upper()

        argument = data[1] if len(data) > 1 else None

        # Analize the command 
        if command == 'FIND': # Find files from other users
            if self.my_state != self.State.IDLE:
                self.doReject("Another operation is in run")
                return
            if argument is None:
                self.doReject("Regex expected")
                return
            self.handleFind(argument)

        elif command == 'FILES': # Polled user returns list of files here
            if self.my_state != self.State.POLLED:
                self.doReject("Got list of files, but there is no request for them")
                return
            #  self.log.debug("Got list of files from {peer!r}", peer=self.transport.getPeer())
            if argument is None:
                self.wait_for_list.errback(RuntimeError("This peer has no such files"))
            else:
                # Return self id and list of files
                self.wait_for_list.callback(':'.join( [str(id(self)), argument] )) 
            # Cleanup 
            self.wait_for_list = None
            self.my_state = self.State.IDLE
            
        elif command == 'GET': # User asks for file
            '''
            This is a routine of connection, whitch needs file
            '''
            # Check if we ready
            if self.my_state != self.State.IDLE:
                self.doReject("Can not give you file at the moment")
                return
            # Get arguments
            if argument is None:
                self.doReject("Expected filename for GET operation")
                return
            try:
                pid, filename = argument.split(':', 1)
            except(IndexError):
                self.doReject("not valid filename")
                return

            # Find the peer
            peer = self.factory.findConnById(pid)

            # Check if we are ready
            if peer is None:
                self.doReject("Sorry, the peer seems afk")
                return
            if peer.my_state != peer.State.IDLE:
                self.doReject("Sorry, the peer seems busy")
                return

            # Ready to begin, don't interrupt us please
            peer.my_state = peer.State.RAW
            self.my_state = self.State.RAW

            peer.file_transfer_peer = self
            self.file_transfer_peer = peer

            peer.send("OBTAIN", filename)
            self.send("RAW", filename)

            self.setRawMode()
            peer.setRawMode()

        elif command == 'ALICE': # User remembers how this project started
            self.send('', "nice girl")

        else:
            self.doReject("not a protocol command: %s" % command)

    def rawDataReceived(self, data):
        '''
        Should be called, if user sends data to peer in response
        '''
        if self.file_transfer_peer is None:
            self.log.warning("RECIEVING RAW DATA WITHOUT PEER TO RESEND") 
            return
        peer = self.file_transfer_peer
        self.log.info("RAW: resending data: %d KB" % len(data))
        peer.transport.write(data)
        if not data.endswith(b'\r\n'):
            # Transmission not done yet
            return
        self.log.info("RAW: Transmission complete")
        peer.setIdleState()
        self.setIdleState()

    def connectionLost(self, reason):
        self.log.info("Connection closed: {peer!r}, {reason!r}", 
                peer=self.transport.getPeer(), reason=reason)
        self.factory.protocols.remove(self)


class MainFactory(protocol.ServerFactory):
    protocol = FSProtocol

    def __init__(self):
        self.protocols = []
        self.log = Logger()

    def getFiles(self, asker, regex):
        ''' 
        Triggered by users to find the files of the others
        Returns string formed as this:
        id1:file1:file2:....:fileN;id2:.....
        (technicly it returns a deferred witch will return this)
        '''
        self.log.info("Starting file searching: {regex!s}",
                regex=regex)
        if len(self.protocols) <= 1:
            return defer.fail(RuntimeError("No one else connected"))

        dlist = [ p.requestFiles(regex) for p in self.protocols if p is not asker ]
        ans = defer.DeferredList(dlist, consumeErrors=True)
        # This is a list of tuples (success,result).
        def concat(l):
            filelist = [ x for r,x in l if r ]
            self.log.debug("Callback with file responses: {dlist!r}, filelist is {filelist!s}", 
                    dlist=l, filelist=filelist)
            return ';'.join(filelist)
        return ans.addCallback(concat)
    
    def findConnById(self, pid):
        self.log.info("Searching for protocol id " + str(pid))
        for p in self.protocols:
            if str(id(p)) == str(pid):
                return p
        return None


#  class MainService(service.Service):
    #  def __init__(self):
        #  self.protocols = []
        #  self.log = Logger()

    #  def knock(self, protocol):
        #  self.protocols.append(protocol)


def main(reactor):
    log = Logger()
    def fe(inp):
        return formatEventAsClassicLogText(inp) + "\n"
    globalLogPublisher.addObserver(FileLogObserver(sys.stdout, fe))

    ds_certData = getModule(__name__).filePath.sibling('darkserver').child('darkserver.pem').getContent()
    rt_certData = getModule(__name__).filePath.sibling('rootCA.d').child('rootCA.crt').getContent()

    ds_cert = ssl.PrivateCertificate.loadPEM(ds_certData)
    rt_cert = ssl.Certificate.loadPEM(rt_certData)

    #  l.info("ds_cert: {ds_cert.inspect()}", ds_cert=ds_cert)
    #  l.info("ds_cert key: ds_cert.privateKey.inspect()", privateKey=ds_cert.privateKey)
    #  l.info("rt_cert: rt_cert.inspect()", rt_cert=rt_cert)

    options = ssl.CertificateOptions(
            certificate=ds_cert.original,
            privateKey=ds_cert.privateKey.original,
            trustRoot=rt_cert,
            verifyDepth=2,
            raiseMinimumTo=ssl.TLSVersion.TLSv1_1)

    #  l.info('Starting service')
    #  f = MainService()
    #  f.options = options

    log.info('Initiating listening')
    #  tlsFactory = TLSMemoryBIOFactory(options, False, f.getFingerFactory())
    #  reactor.listenTCP(8123, tlsFactory)
    reactor.listenSSL(8123, MainFactory(), options) # FingerFactory({b'alice' : b'nice girl'})) #, ds_cert.options(rt_cert))
    #  reactor.listenTCP(8123, f.getFingerFactory())
    return defer.Deferred()

if __name__ == '__main__':
    import fsserver
    #  l.info('Starting')
    task.react(fsserver.main)


#  fingerEndpoint = endpoints.serverFromString(reactor, "tcp:8123")

#  ff = FingerFactory({b'alice' : b'nice girl'})
#  fsf = FSetterFactory(ff)

#-----------------

#  application = service.Application('fingertest', uid=1, gid=1)
#  serviceCollection = service.IServiceCollection(application)

#  strports.service("tcp:8123", f.getFingerFactory()).setServiceParent(serviceCollection)
#  strports.service("tcp:2079", f.getFSetterFactory()).setServiceParent(serviceCollection)

#-----------------


