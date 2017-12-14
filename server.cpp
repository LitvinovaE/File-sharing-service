#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) : QDialog(parent), ui(new Ui::Server)
{
    ui->setupUi(this);
    ui->password_edit->setEchoMode(QLineEdit::Password);            // show "*" instead of symbols
}

Server::~Server()
{
    delete ui;
}

void Server::on_button_OK_clicked()             // TO DO!!!
{
    QString server_name = ui->server_edit->text();
    QString password = ui->password_edit->text();
    if(server_name_is_correct(server_name))             // data send to a server
    {
        int error_code = send_data_for_connection(server_name, password);
        switch (error_code)
        {
        case 0:                                         // connection is ok, close this window and show the next
            close();
            break;
        case 1:
            QMessageBox::information(0, "Error", "Something's wrong!");
            break;
        default:
            break;
        }
        ui->server_edit->clear();
        ui->password_edit->clear();
    }
    else                                                // show a hint and clear fields
    {
        ui->server_edit->clear();
        ui->password_edit->clear();
        ui->server_edit->setStyleSheet("border: 2px solid red");
        QToolTip::showText(ui->server_edit->mapToGlobal(QPoint()), tr("Only letters and numbers!"));
    }
}

void Server::on_button_Cancel_clicked()
{
    close();
}

bool Server::server_name_is_correct(QString str)
{
    if(str.isEmpty())
        return false;

    for(auto &i : str)
    {
        if(!i.isLetterOrNumber())
            return false;
    }
    return true;
}

int Server::send_data_for_connection(QString server_name, QString password)         // TO DO!!!
{
    return 0;
}
