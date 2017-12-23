#include "mainwindow.h"
#include "login.h"
#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server s;
    MainWindow main_window;
    s.show();
    QObject::connect(&s, &Server::sendLoginData, &main_window, &MainWindow::make_connection);

    return a.exec();
}
