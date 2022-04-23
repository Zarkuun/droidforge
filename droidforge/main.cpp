#include "parseexception.h"
#include "mainwindow.h"
#include "patch.h"

#include "unistd.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString initialFilename;
    if (argc > 1)
        initialFilename = argv[1];
    else
        initialFilename = "/Users/mk/git/droidforge/testpatch.ini";

    MainWindow mainWindow(initialFilename);
    mainWindow.show();
    return a.exec();
}
