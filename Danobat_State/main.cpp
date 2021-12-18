#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.setStyleSheet("background-color: rgb(255, 255, 255);");
    w.showMaximized();
    return a.exec();
}
