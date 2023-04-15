//#include "mainwindow.h"
#include "startmenu.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w(9);
    //w.show();

    StartMenu* startMenu = new StartMenu();
    startMenu->show();

    return a.exec();
}
