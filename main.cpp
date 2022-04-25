#include "mainwindow.h"
#include "Utils.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    RemoveDisplaySleep();

    QApplication a(argc, argv);
    MainWindow w;
    w.showFullScreen();
    w.AdjustGeometry();
    w.InitWidgets();
    return a.exec();
}
