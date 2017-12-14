#include "request.h"
#include "ui_request.h"

Request::Request(QWidget *parent) : QDialog(parent), ui(new Ui::Request)
{
    ui->setupUi(this);
}

Request::~Request()
{
    delete ui;
}

void Request::decline_request()
{

}

void Request::accept_request()
{

}

void Request::set_label(QString user_name, QString file_name)
{
    ui->request_label->setText("Somebody wants to download your file.\n\nUser: " + user_name + "\nFile: " + file_name);
}

void Request::on_button_Decline_clicked()
{
    close();
    decline_request();
}

void Request::on_button_Accept_clicked()
{
    close();
    accept_request();
}
