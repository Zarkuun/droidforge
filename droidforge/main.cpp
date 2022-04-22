#include "generalparseexception.h"
#include "mainwindow.h"
#include "patch.h"

#include "unistd.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
