#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>
#include <QToolTip>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
    bool is_exist();

private slots:
    void on_button_OK_clicked();

private:
    Ui::Login *ui;
    bool login_is_correct(QString str);
};

#endif // LOGIN_H
