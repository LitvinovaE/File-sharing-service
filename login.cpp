#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) : QDialog(parent), login_ui(new Ui::Login)
{
    this->login_ui->setupUi(this);
}

Login::~Login()
{
    delete this->login_ui;
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
    auto login_edit = this->login_ui->login_edit;
    QString login = login_edit->text();
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
        emit login_changed(login);
    }
    else                                        // clear field and show a hint
    {
        login_edit->clear();
        login_edit->setStyleSheet("border: 2px solid red");
        QToolTip::showText(login_edit->mapToGlobal(QPoint()), tr("Only letters and numbers!"));
    }
}
