#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->table_widget->setRowCount(1);                              // draw a table
    ui->table_widget->setColumnCount(4);
    ui->table_widget->setColumnWidth(0, 572); /* another value is 539 if more than */
    ui->table_widget->setColumnWidth(1, 100);
    ui->table_widget->setColumnWidth(2, 100);
    ui->table_widget->setColumnWidth(3, 170);
    ui->table_widget->setHorizontalHeaderLabels(QStringList() << "Name" << "Size" << "Date" << "Owner");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_my_requests_clicked()
{

}

void MainWindow::on_button_find_clicked()
{

}
