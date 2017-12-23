#include "mainwindow.h"
#include "login.h"
#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*Login w;
    if (!w.is_exist())
        w.show();*/
    /*Server s;
    s.show();*/
    MainWindow main_window;
    main_window.show();

    return a.exec();
}
