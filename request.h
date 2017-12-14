#ifndef REQUEST_H
#define REQUEST_H

#include <QDialog>
#include <QString>

namespace Ui {
class Request;
}

class Request : public QDialog
{
    Q_OBJECT

public:
    explicit Request(QWidget *parent = 0);
    ~Request();
    void decline_request();
    void accept_request();
    void set_label(QString user_name, QString file_name);

private slots:
    void on_button_Decline_clicked();
    void on_button_Accept_clicked();

signals:
    void request_created(QString request);

private:
    Ui::Request *ui;
};

#endif // REQUEST_H
