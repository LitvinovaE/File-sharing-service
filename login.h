#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QString>
#include <QToolTip>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    bool is_exist();

private slots:
    void on_button_OK_clicked();

signals:
    void login_changed(QString login);

private:
    Ui::Login *login_ui;
    bool login_is_correct(QString str);
};

#endif // LOGIN_H
