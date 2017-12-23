#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , login_window(Q_NULLPTR)
    , sslclient(new Client(this))
    , file_list(this)
//    , m_share_root("/tmp/")
{
    ui->setupUi(this);

    ui->table_widget->setRowCount(1);                              // draw a table
    ui->table_widget->setColumnCount(3);
    ui->table_widget->setColumnWidth(0, 572);
    ui->table_widget->setColumnWidth(1, 185);
    ui->table_widget->setColumnWidth(2, 185);
    ui->table_widget->setHorizontalHeaderLabels(QStringList() << "Name" << "Size" << "Date");

    // Openning connection
    // this should be done somewhere else
    // e.g. in Login
    connect(sslclient, &Client::clientError, this, [=](QString error){
        QMessageBox::information(this, "sslclient error", error);
    });
    connect(sslclient, &Client::gotResponse, this, [=](const QString &list) {
        file_list.parseString(list);
        draw_table();
    });
    connect(sslclient, &Client::gotReject, this, [=](const QString &msg) {
        QMessageBox::information(this, "Server REJECT", msg);
    });

    sslclient->makeConnection("192.168.22.103", 8123);

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
        return;
    }
#ifdef RISKY
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
    new_request(QString("John"), QString("bloknot.txt"));
#else
    sslclient->sendFINDrequest(file_mask);
#endif
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

void MainWindow::draw_table(const FileModel::DataType &file_table)
{
    int num_elem = file_table.length();
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

void MainWindow::draw_table()
{
    draw_table(file_list.getData());
}

void MainWindow::new_request(QString user_name, QString file_name)
{
    request_window = new Request(this);
    QObject::connect(this->request_window, SIGNAL(request_created(QString)), this, SLOT(slot_request_created(QString)));
    request_window->set_label(user_name, file_name);
    request_window->show();
}

void MainWindow::on_actionChange_login_triggered()
{
    this->login_window = new Login(this);
    QObject::connect(this->login_window, SIGNAL(login_changed(QString)), this, SLOT(slot_login_changed(QString)));
    this->login_window->show();
}

void MainWindow::slot_login_changed(QString login)
{
    delete this->login_window;
}

void MainWindow::slot_request_created(QString request)
{

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(0, "About", "The program is designed to exchange files over the network. \
With its help you can download files of users connected to the same server, and also you can respond to user \
requests allowing or rejecting access to your files.");
}

void MainWindow::on_actionCreators_triggered()
{
    QMessageBox::information(0, "Creators", "This project is created by:\n\nBondarev Mihail"
                                            "\nLitvinova Evgeniya\nOrlov Oleg\n\n2017, MIPT");
}

void MainWindow::on_table_widget_doubleClicked(const QModelIndex &index)
{
    int row_clicked = index.row();
    auto file_row = file_list.getData()[row_clicked];
    sslclient->sendGETrequest(file_row[3], file_row[0]);
}
