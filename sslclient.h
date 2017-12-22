#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QSslSocket>
#include <QFileInfoList>
#include <QFile>

QT_BEGIN_NAMESPACE
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QsslSocket;
class QNetworkSession;
QT_END_NAMESPACE

//! [0]
class Client : public QDialog
{
    Q_OBJECT

public:
    Client(QWidget *parent = 0);
    void sendFINDrequest(QString );
    void sendFoundFiles(const QFileInfoList & );

private slots:
    void requestNewFortune();
    void readFortune();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetFortuneButton();
    void sessionOpened();
    void discon();

signals:
    void ServerError(QString error);
    void reply(std::vector<std::string> list);
    void clientError(const char*);
    void ListReqFiles(QString**);
    void SendError(const char*);

private:
    void loadPfxCertifcate(QString certPath, QString passphrase);
    void sendFile(QString fileName);
    void sendPartOfFile();
    void receiveFile(QString fileName);
    void receiveFile();

    QFile *FileForSend;
    QFile *receivedFile;
    qint64 sizeReceivedData;

    QLabel *hostLabel;
    QLabel *portLabel;
    QComboBox *hostCombo;
    QLineEdit *portLineEdit;
    QLabel *statusLabel;
    QPushButton *getFortuneButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;

    QSslSocket*sslSocket;
    QString currentFortune;
    quint16 blockSize;

    QNetworkSession *networkSession;
};
//! [0]

#endif
