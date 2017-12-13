#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "login.h"
#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QToolTip>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_button_find_clicked();
    void on_actionChange_download_share_folders_triggered();
    void on_actionChange_login_triggered();
    void on_actionAbout_triggered();
    void on_actionCreators_triggered();

private:
    Ui::MainWindow *ui;
    void draw_table(QString **file_table, int num_elem);
};

#endif // MAINWINDOW_H
