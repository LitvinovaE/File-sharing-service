#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "login.h"
#include "request.h"
#include "server.h"
#include "sslclient.h"
#include "filemodel.h"
#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QToolTip>
#include <QMessageBox>
#include <iostream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString getShareRoot() const { return m_share_root; }

private slots:
    void on_button_find_clicked();
    void on_actionChange_login_triggered();
    void on_actionAbout_triggered();
    void on_actionCreators_triggered();
    void slot_login_changed(QString login);
    void slot_request_created(QString request);

    void on_table_widget_doubleClicked(const QModelIndex &index);
public slots:
    void make_connection(QString server, QString password);

private:
    Ui::MainWindow *ui;
    Login *login_window;
    Request *request_window;
    Client *sslclient;
    FileModel file_list;

    QString m_share_root;

    void draw_table(QString **file_table, int num_elem);
    void draw_table(const FileModel::DataType &file_table);
    void draw_table();
    void new_request(QString user_name, QString file_name);
};

#endif // MAINWINDOW_H
