#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QString>
#include <QToolTip>
#include <QMessageBox>

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

private slots:
    void on_button_OK_clicked();
    void on_button_Cancel_clicked();

private:
    Ui::Server *ui;
    bool server_name_is_correct(QString str);
    int send_data_for_connection(QString server_name, QString password);       // return server's answer about connection
};

#endif // SERVER_H
