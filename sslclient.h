#ifndef CLIENT_H
#define CLIENT_H

#include <QSslSocket>
#include <QFileInfoList>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QsslSocket;
class QNetworkSession;
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    Client(QWidget *parent = 0);
    void makeConnection(QString ip, int port);
    void sendFINDrequest(QString regexpr);

private slots:
    void discon();
    void readFortune();
    void displayError(QAbstractSocket::SocketError);

signals:
    void gotReject(QString error);
    void gotResponse(QString);
    void clientError(QString);

private:
    void processRequest(const QFileInfoList & );
    void loadPfxCertifcate(QString certPath, QString passphrase);

    void sendFile(QString fileName);
    void receiveFile(QString fileName);

    QFile *FileForSend;
    QFile *receivedFile;
    qint64 sizeReceivedData;

    QSslSocket sslSocket;
    QString currentFortune;
    quint16 blockSize;
};

#endif
