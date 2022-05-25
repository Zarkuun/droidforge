#include "parseexception.h"
#include "mainwindow.h"
#include "patch.h"

#include "unistd.h"
#include "cablecolorizer.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>

CableColorizer *the_cable_colorizer;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("droidforge");
    app.setApplicationDisplayName("DROID Forge");
    app.setOrganizationName("Der Mann mit der Maschine");
    app.setOrganizationDomain("dmmdm.de");

    QString initialFilename;
    if (argc > 1)
        initialFilename = argv[1];

    CableColorizer cableColorizer;
    the_cable_colorizer = &cableColorizer;

    MainWindow mainWindow(initialFilename);
    mainWindow.show();
    QDir::setCurrent(mainWindow.userPatchDirectory().absolutePath());

    QSettings settings;
    if (settings.contains("mainwindow/size"))
        mainWindow.resize(settings.value("mainwindow/size").toSize());

    if (settings.contains("mainwindow/position"))
        mainWindow.move(settings.value("mainwindow/position").toPoint());

    return app.exec();
}
