#include <QtNetwork>
#include <QDir>
#include <QtDebug>
#include <QStringList>
#include <string>
#include <QFileInfoList>
#include <QMessageBox>

#include "sslclient.h"

#define SIZE_BLOCK_FOR_SEND_FILE 1024

QFileInfoList find_files(QString path_name, QString reg_expr)
{
    QDir directory(path_name);

    QFileInfoList files = directory.entryInfoList(QStringList(reg_expr), QDir::Files);
    return files;
}

void genPfxCertifcate(QString certPath)
{
    return;
    if(!QFileInfo::exists(certPath + "child.key"))
    {
        qDebug()<< "ssl gen rsa";
        QString keygen = "openssl genrsa -out child.key 2048" ;
        system(keygen.toStdString().c_str());
    }
    if(!QFileInfo::exists(certPath + "child.crt"))
    {
        qDebug() <<"ssl gen csr";
        system("openssl req -new -key child.key -out child.csr -subj \"/C=US/ST=Moscow/L=Moscow/O=Company Name/OU=Org/CN=www.com\"");
    }
}

void Client::loadPfxCertifcate(QString certPath, QString passphrase)
{
    genPfxCertifcate(certPath);
    sslSocket.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslSocket.setProtocol(QSsl::TlsV1_2);

    QList<QSslCertificate> importedCerts = QSslCertificate::fromPath(certPath + "rootCA.crt");
    qDebug() << "read CA certificate";
    if (importedCerts.isEmpty()) {
        throw std::runtime_error("no CA sertificates found");
    }

    auto cert_list = QSslCertificate::fromPath(certPath + "child.crt");
    qDebug() << "read child certificate";
    if (cert_list.isEmpty()) {
        throw std::runtime_error("no sertificates found");
    }

    sslSocket.setCaCertificates(importedCerts);
    sslSocket.setLocalCertificate(cert_list.first());
    qDebug() << sslSocket.localCertificate().toText();
    sslSocket.setPrivateKey(certPath + "child.key", QSsl::Rsa, QSsl::Pem, QByteArray::fromStdString(passphrase.toStdString()));

    qDebug() << sslSocket.privateKey().toPem();
}

Client::Client(QWidget *parent)
    : QWidget(parent)
//    , networkSession(0)
    , sslSocket(this)
    , shareRoot("/home/mikle/share/")
    , FileForSend(nullptr)
    , isRAW(false)
{
    // TODO: this hardcode needs to be changed!
    QString certs_path = "/home/mikle/certificates/";
    loadPfxCertifcate(certs_path, "12345");

    qDebug() << sslSocket.localCertificate().subjectInfo(QSslCertificate::CommonName).join(QLatin1Char(' '));

    connect(&sslSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));

    connect(&sslSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));

    connect(&sslSocket, SIGNAL(disconnected()), this, SLOT(discon()));
    connect(this, &QObject::destroyed, &sslSocket, &QAbstractSocket::disconnectFromHost);
}

void Client::makeConnection(QString ip, int port)
{
        sslSocket.abort();
        blockSize = 0;

        qDebug() << "Connecting to server: " << ip << port;
        sslSocket.connectToHostEncrypted(ip, port);
}

void Client::readFortune()
{
//    if(linelen < 0) qDebug() <<"\nreading failed";
//    else qDebug() << request;
    if (isRAW)
    {
        receiveFile();
        return;
    }
    QString serverSend(sslSocket.readLine());
    serverSend.remove("\r\n");

    qDebug() << serverSend;

    if(serverSend.startsWith("REQUEST "))
    {
        processRequest(find_files(shareRoot, serverSend.remove(0, 8)));
    }
    else if(serverSend.startsWith("REJECT "))
    {
        emit gotReject(serverSend.remove(0, 7));
    }
    else if(serverSend.startsWith("RESPONSE "))
    {
        emit gotResponse(serverSend.remove(0, 9));
    }
    else if (serverSend.startsWith("OBTAIN "))
    {
        sendFile(shareRoot + serverSend.remove(0, 7));
    }
    else if (serverSend.startsWith("RAW "))
    {
        isRAW = true;
        currentDownload =shareRoot + "download/" + serverSend.remove(0, 4);
        receiveFile();
    }
    else
    {
        emit clientError("Not a valid command: " + serverSend);
    }
}

void Client::sendFINDrequest(QString regexpr)
{
    QString request = "FIND " + regexpr;
    qint64 linelen = sslSocket.write(request.toUtf8() + "\r\n");
    if(linelen < 0)
        emit clientError("can't send FIND request");
}

void Client::sendGETrequest(QString peer, QString file)
{
    QString request = QString("GET %1:%2").arg(peer, file);
    qint64 linelen = sslSocket.write(request.toUtf8() + "\r\n");
    if(linelen < 0)
        emit clientError("can't send GET request");
}

void Client::processRequest(const QFileInfoList &foundFiles)
{
    QString request = "FILES " ;
    for(auto it : foundFiles)
    {
        QString file = QString("%1!%2!%3")
                .arg(it.fileName())
                .arg(QString::number(it.size() / 1024) + "Kb")
                .arg(it.lastModified().toString("dd.mm.yy"));
        request += file + ':';
    }
    request.remove(QRegExp(":$"));
    if (request.endsWith(':'))
        qDebug() << "it sucks";

    qint64 linelen = sslSocket.write(request.toUtf8() + "\r\n");
    if(linelen < 0)
        emit clientError("can'sendFoundFilest send found files request");
}

void Client::displayError(QAbstractSocket::SocketError)
{
    emit clientError(sslSocket.errorString());
}

void Client::discon()
{
    QMessageBox::information(this, "Disconnected", "if you see this, Pinkie tells lie");
}

void Client::sendFile(QString fileName)
{
#ifdef OLEG
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_2);

    data.append(FileForSend->readAll());
    FileForSend->close();
    out.device()->seek(0);

    int written = 0;
    while(written < data.size())
        written += sslSocket.write(data);
#else
    FileForSend = new QFile(fileName);
    if(!FileForSend->open(QFile::ReadOnly))
    {
        qDebug()<< QString("File not can open for read");
        return;
    }

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);

    out << (qint64)0;
    out << FileForSend->readAll();
    out.device()->seek(0);
    out << (qint64)(data.size() - sizeof(qint64));

    FileForSend->close();
    delete FileForSend;
    FileForSend = nullptr;

    int written = 0;
    while(written < data.size() + sizeof(qint64))
        written += sslSocket.write(data);
    sslSocket.write("\r\n");
    sslSocket.waitForBytesWritten();
#endif
}

void Client::receiveFile()
{
    QDataStream in(&sslSocket);
    in.setVersion(QDataStream::Qt_5_5);

    // QByteArray line = sslSocket.readAll();
    if (blockSize == 0)
    {
        if (sslSocket.bytesAvailable() < sizeof(blockSize))
            return;
        // Read size of data stream
        in >> blockSize;
    }
    if (sslSocket.bytesAvailable() < blockSize)
        // Wait for the whole data
        return;

    QByteArray line;
    in >> line;
    QFile target(currentDownload);
    if (!target.open(QIODevice::WriteOnly))
    {
            qDebug() << "Can't open file for written";
            return;
    }
    target.write(line);
    target.close();
    isRAW = false;
}

