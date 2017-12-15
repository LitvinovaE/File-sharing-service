#include <QtWidgets>
#include <QtNetwork>

#include "sslclient.h"


void Client::loadPfxCertifcate(QString certPath, QString passphrase)
{
//    qDebug() << "Load " + certPath + "child.crt";
//    QList<QSslCertificate> certificate = QSslCertificate::fromPath(certPath + "child.crt");
//    QSslCertificate cert = certificate.first();

//    QFile certFile(certPath + "child.key");
//    certFile.open(QFile::ReadOnly);
//    QSslKey key(&certFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray::fromStdString(passphrase.toStdString()));
//    certFile.close();

    sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    QList<QSslCertificate> importedCerts = QSslCertificate::fromPath(certPath + "rootCA.crt");
    qDebug() << "read CA certificate";

//    qDebug() << "Imported cert:\n"
//             << "certificate:\n" << cert.toText() << "\n----------\n"
//             << "key:\n" << key.toPem() << "\n----------\n"
//             << "importedcerts:\n" << importedCerts.first().toText() << "\n----------\n";
    sslSocket->setProtocol(QSsl::TlsV1_2);
    QSslCertificate cert = QSslCertificate::fromPath(certPath + "child.crt").first();
    qDebug() << "read certificate";
    this->sslSocket->setCaCertificates(importedCerts);
    this->sslSocket->setLocalCertificate(cert);
    qDebug() << sslSocket->localCertificate().toText();
    this->sslSocket->setPrivateKey(certPath + "child.key", QSsl::Rsa, QSsl::Pem, QByteArray::fromStdString(passphrase.toStdString()));

}

Client::Client(QWidget *parent)
:   QDialog(parent), networkSession(0)
{

    hostLabel = new QLabel(tr("&Server name:"));
    portLabel = new QLabel(tr("S&erver port:"));

    hostCombo = new QComboBox;
    hostCombo->setEditable(true);
    // find out name of this machine
    QString name = "0.tcp.ngrok.io";//QHostInfo::localHostName();
    if (!name.isEmpty()) {
        hostCombo->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            hostCombo->addItem(name + QChar('.') + domain);
    }
    if (name != QString("localhost"))
        hostCombo->addItem(QString("localhost"));
    // find out IP addresses of this machine
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // add non-localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (!ipAddressesList.at(i).isLoopback())
            hostCombo->addItem(ipAddressesList.at(i).toString());
    }
    // add localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i).isLoopback())
            hostCombo->addItem(ipAddressesList.at(i).toString());
    }

    portLineEdit = new QLineEdit;
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    hostLabel->setBuddy(hostCombo);
    portLabel->setBuddy(portLineEdit);

    statusLabel = new QLabel(tr("This examples requires that you run the "
                                "Fortune Server example as well."));

    getFortuneButton = new QPushButton(tr("Get Fortune"));
    getFortuneButton->setDefault(true);
    getFortuneButton->setEnabled(false);

    quitButton = new QPushButton(tr("Quit"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getFortuneButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

//! [1]
    sslSocket = new QSslSocket(this);


    QString certs_path = "D:/Oleg/programmin/task1/seq/tc/testchild/";

    loadPfxCertifcate(certs_path, "12345");
    qDebug() << sslSocket->localCertificate().subjectInfo(QSslCertificate::CommonName).join(QLatin1Char(' '));


//! [1]

    connect(hostCombo, SIGNAL(editTextChanged(QString)),
            this, SLOT(enableGetFortuneButton()));
    connect(portLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableGetFortuneButton()));
    connect(getFortuneButton, SIGNAL(clicked()),
            this, SLOT(requestNewFortune()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
//! [2] //! [3]
    connect(sslSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));
//! [2] //! [4]
    connect(sslSocket, SIGNAL(error(QAbstractSocket::SocketError)),
//! [3]
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(sslSocket, SIGNAL(disconnected()), this, SLOT(discon()));
//! [4]

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    setLayout(mainLayout);

    setWindowTitle(tr("Fortune Client"));
    portLineEdit->setFocus();

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        getFortuneButton->setEnabled(false);
        statusLabel->setText(tr("Opening network session."));
        networkSession->open();


    }
//! [5]
}
//! [5]

//! [6]
void Client::requestNewFortune()
{
     static bool conn = false;

    getFortuneButton->setEnabled(false);
    if(!conn)
    {
        blockSize = 0;
        sslSocket->abort();
//! [7]
        qDebug() << "Connecting to server: "
                 << hostCombo->currentText()
                 << portLineEdit->text().toInt();
        sslSocket->connectToHostEncrypted(hostCombo->currentText(),
                                 portLineEdit->text().toInt());
        getFortuneButton->setText("alice");
        conn = !conn;
    }
    else
    {

        qint64 linelen = sslSocket->write(QString("alice").toUtf8() + "\r\n");
        if(linelen < 0)
            qDebug() <<"\nwriting failed";
        else
             qDebug() <<"send";
    }
//! [7]
}
//! [6]

//! [8]
void Client::readFortune()
{
//! [9]


    char request[256];
    //in >> nextFortune;

    qint64 linelen = sslSocket->readLine(request, sizeof(request));

    if(linelen < 0) qDebug() <<"\nreading failed";
    //! [11]


//! [12]

//! [9]
    statusLabel->setText(request);
    getFortuneButton->setEnabled(true);
}
//! [12]

//! [13]
void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(sslSocket->errorString()));
    }

    getFortuneButton->setEnabled(true);
}
//! [13]

void Client::enableGetFortuneButton()
{
    getFortuneButton->setEnabled((!networkSession || networkSession->isOpen()) &&
                                 !hostCombo->currentText().isEmpty() &&
                                 !portLineEdit->text().isEmpty());

}
void Client::discon()
{
     statusLabel->setText("disconnected");
}

void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    statusLabel->setText(tr("This examples requires that you run the "
                            "Fortune Server example as well."));

    enableGetFortuneButton();
}

