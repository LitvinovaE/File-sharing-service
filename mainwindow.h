#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

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
    void on_button_my_requests_clicked();
    void on_button_find_clicked();

private:
    Ui::MainWindow *ui;
    void draw_table(QVector<QVector<QString>> table_vectors);
};

#endif // MAINWINDOW_H
