#include "mainwindow.h"
#include "droidforge.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    DroidForge forge;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
