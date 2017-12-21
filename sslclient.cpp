#include <QtWidgets>
#include <QtNetwork>
#include <string>

#include "sslclient.h"

#define SIZE_BLOCK_FOR_SEND_FILE 1024

void Client::loadPfxCertifcate(QString certPath, QString passphrase)
{

    qDebug()<< "ssl gen rsa";
    QString keygen = "openssl genrsa -out child.key 2048 2>/dev/null" ;
    system(keygen.toStdString().c_str());
    qDebug() <<"ssl gen rootCA";
    system("openssl req -x509 -newkey rsa:2048 -keyout my.key -out rootCA.crt -days 365 -subj \"/C=US/ST=Oregon/L=Portland/O=Company Name/OU=Org/CN=www.com\" 2>/dev/null");

    sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    QList<QSslCertificate> importedCerts = QSslCertificate::fromPath(certPath + "rootCA.crt");
    qDebug() << "read CA certificate";

    sslSocket->setProtocol(QSsl::TlsV1_2);
    QSslCertificate cert = QSslCertificate::fromPath(certPath + "child.crt").first();
    qDebug() << "read certificate";
    this->sslSocket->setCaCertificates(importedCerts);
    this->sslSocket->setLocalCertificate(cert);
    qDebug() << sslSocket->localCertificate().toText();
    this->sslSocket->setPrivateKey("child.key", QSsl::Rsa, QSsl::Pem, QByteArray::fromStdString(passphrase.toStdString()));

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


    sslSocket = new QSslSocket(this);


    QString certs_path = "/home/oleg/Qt/projects/seq/tc/testchild/";

    loadPfxCertifcate(certs_path, "12345");
    qDebug() << sslSocket->localCertificate().subjectInfo(QSslCertificate::CommonName).join(QLatin1Char(' '));


    connect(hostCombo, SIGNAL(editTextChanged(QString)),
            this, SLOT(enableGetFortuneButton()));
    connect(portLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(enableGetFortuneButton()));
    connect(getFortuneButton, SIGNAL(clicked()),
            this, SLOT(requestNewFortune()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(sslSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));

    connect(sslSocket, SIGNAL(error(QAbstractSocket::SocketError)),

            this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(sslSocket, SIGNAL(disconnected()), this, SLOT(discon()));


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

}


void Client::requestNewFortune()
{
     static bool conn = false;
    getFortuneButton->setEnabled(false);
    if(!conn)
    {
        blockSize = 0;
        sslSocket->abort();

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

}

void Client::readFortune()
{
//    if(linelen < 0) qDebug() <<"\nreading failed";
//    else qDebug() << request;
    QString serverSend(sslSocket->readLine());

    if(serverSend.startsWith("REQUEST ")) {
        emit findByRegexp(serverSend.remove(0, 8));
    }
    else if(serverSend.startsWith("REJECT "))
    {
        emit ServerError(serverSend.remove(0, 7));
    }
    else if(serverSend.startsWith("REPLY "))
    {
        serverSend = serverSend.remove(0, 6);
        QStringList files = serverSend.split(';');
        QString *f[3];
        for (auto i: files)
        {
            QStringList ftmp = i.split(':');
            QString tmpID = ftmp[0];
            ftmp.pop_front();

            for (int i = 1; i < 3; ++i)
                f[i] = new QString[ftmp.size()];
            for(int i = 1; i < ftmp.size(); ++i)
            {
                f[2][i] = ftmp[i];
            }

        }
        emit ListReqFiles(f);
    }
//    statusLabel->setText(request);
    getFortuneButton->setEnabled(true);
}

void Client::sendFINDrequest(QString regexpr)
{
    QString request = "FIND " + regexpr;
    qint64 linelen = sslSocket->write(request.toUtf8() + "\r\n");
    if(linelen < 0)
        emit SendError("can't send FIND request");
}

void Client::sendFoundFiles(QVector<QFile> foundFiles)
{
    QString request = "FILES " ;
    QFileInfo info;
    int j = 0;
    for(auto i: foundFiles)
    {
        info.setFile(i);
        qint64 size = info.size();
        request += info.fileName() + "!" ;
        while (size > 100)
        {
            size /= 1024;
            ++j;
        }
        request += QString::number(size) + (j == 0)?"b":(j == 1)?"Kb":(j == 2)?"Mb":"Gb" + "!" + info.fileTime(QFileDevice::FileModificationTime).toString("dd.mm.yy");
    }

    qint64 linelen = sslSocket->write(request.toUtf8() + "\r\n");
    if(linelen < 0)
        emit SendError("can't send found files request");
}

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

void Client::sendFile(QString fileName)
{

    FileForSend = new QFile(fileName);
    if(FileForSend->open(QFile::ReadOnly))
    {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_2);

        data.append(FileForSend->readAll());
        FileForSend->close();
        out.device()->seek(0);

        int written = 0;
        while(written < data.size())
            written += sslSocket->write(data);
    } else
    {
        qDebug()<< QString("File not can open for read");
        return;
    }
}

void Client::receiveFile(QString fileName)
{
    QDataStream in(sslSocket);
    QFile target(fileName);
    if (!target.open(QIODevice::WriteOnly))
    {
            qDebug() << "Can't open file for written";
            return;
    }
    QByteArray line = sslSocket->readAll();
    target.write(line);
    target.close();
}

