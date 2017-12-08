#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) : QWidget(parent), ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

bool Login::login_is_correct(QString str)
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

bool Login::is_exist()
{
    QFile login_file("LOGIN");
    return login_file.exists();
}

void Login::on_button_OK_clicked()
{
    QString login = ui->login_edit->text();
    if(login_is_correct(login))                // login saved in the file "LOGIN"
    {
        QFile login_file("LOGIN");
        if(login_file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&login_file);
            stream << login;
            login_file.close();
        }
        close();
    }
    else                                        // clear field and show a hint
    {
        ui->login_edit->clear();
        ui->login_edit->setStyleSheet("border: 2px solid red");
        QToolTip::showText(ui->login_edit->mapToGlobal(QPoint()), tr("Only letters and numbers!"));
    }
}
