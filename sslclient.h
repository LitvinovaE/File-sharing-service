#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QSslSocket>

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

private slots:
    void requestNewFortune();
    void readFortune();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetFortuneButton();
    void sessionOpened();
    void discon();

private:
    void loadPfxCertifcate(QString certPath, QString passphrase);

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
