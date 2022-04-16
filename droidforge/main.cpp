#include "mainwindow.h"
#include "patch.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow mainWindow;

    if (!mainWindow.loadPatch("/Users/mk/git/droidforge/testpatch.ini"))
    {
        QMessageBox box;
        box.setText("Fehler!");
        box.exec();
        QApplication::quit();
    }

    mainWindow.resize(800,1000);
    mainWindow.move(1200, 0);
    mainWindow.show();
    return a.exec();
}
