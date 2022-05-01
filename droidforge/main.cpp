#include "parseexception.h"
#include "mainwindow.h"
#include "patch.h"

#include "unistd.h"

#include <QApplication>
#include <QDir>
#include <QSettings>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Der Mann mit der Maschine");
    QCoreApplication::setOrganizationDomain("dmmdm.de");
    QCoreApplication::setApplicationName("Droid Forge");

    QApplication a(argc, argv);
    QString initialFilename;
    if (argc > 1)
        initialFilename = argv[1];
    else
        initialFilename = "/Users/mk/git/droidforge/testpatch.ini";

    MainWindow mainWindow(initialFilename);
    mainWindow.show();
    QSettings settings;

    if (settings.contains("mainwindow/size"))
        mainWindow.resize(settings.value("mainwindow/size").toSize());

    if (settings.contains("mainwindow/position"))
        mainWindow.move(settings.value("mainwindow/position").toPoint());

    return a.exec();
}
