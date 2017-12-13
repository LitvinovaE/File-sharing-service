#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->table_widget->setRowCount(1);                              // draw a table
    ui->table_widget->setColumnCount(4);
    ui->table_widget->setColumnWidth(0, 572);
    ui->table_widget->setColumnWidth(1, 100);
    ui->table_widget->setColumnWidth(2, 100);
    ui->table_widget->setColumnWidth(3, 170);
    ui->table_widget->setHorizontalHeaderLabels(QStringList() << "Name" << "Size" << "Date" << "Owner");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_find_clicked()       //TO DO!!!
{
    QString file_mask = ui->file_mask_edit->text();
    if(file_mask.isEmpty())
    {
        QToolTip::showText(ui->file_mask_edit->mapToGlobal(QPoint()), tr("Please, enter the mask of files"));
    }
    QString **file_table = new QString* [30];
    int num_elem = 30;
    for(int j = 0; j < num_elem; j++)
    {
        QString *str = new QString [4];
        str[0] = "NNNNAmmmmme";
        str[1] = "456.25";
        str[2] = "13.12.2017";
        str[3] = "John1234";
        file_table[j] = str;
    }
    draw_table(file_table, num_elem);
}

void MainWindow::draw_table(QString **file_table, int num_elem)
{
    ui->table_widget->setRowCount(num_elem);
    if(num_elem > 21)
        ui->table_widget->setColumnWidth(0, 547);
    else
        ui->table_widget->setColumnWidth(0, 563);
    int row_number = ui->table_widget->rowCount();
    int column_number = ui->table_widget->columnCount();
    for(int i = 0; i < row_number; i++)
        for(int j = 0; j < column_number; j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(tr("%1").arg(file_table[i][j]));
            ui->table_widget->setItem(i, j, item);
        }
}

void MainWindow::on_actionChange_download_share_folders_triggered()
{

}

void MainWindow::on_actionChange_login_triggered()
{
    Login w;
    w.show();
}

void MainWindow::on_actionAbout_triggered()
{

}

void MainWindow::on_actionCreators_triggered()
{
    QMessageBox::information(0, "Creators", "This project is created by:\n\nBondarev Mihail\nLitvinova Evgeniya\nOrlov Oleg\n\n2017, MIPT");
}
