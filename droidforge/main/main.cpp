#include "globals.h"
#include "parseexception.h"
#include "mainwindow.h"
#include "patch.h"
#include "cablecolorizer.h"
#include "updatehub.h"
#include "clipboard.h"
#include "colorscheme.h"
#include "usermanual.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationVersion(APPLICATION_VERSION);
    app.setApplicationDisplayName(APPLICATION_NAME);
    app.setOrganizationName("Der Mann mit der Maschine");
    app.setOrganizationDomain("dmmdm.de");

    // Create and change to user's patch directory
    QDir dir = QDir::homePath();
    if (!dir.cd(PATCH_DIRECTORY_NAME)) {
        dir.mkdir(PATCH_DIRECTORY_NAME);
    }
    dir.cd(PATCH_DIRECTORY_NAME);
    QDir::setCurrent(dir.absolutePath());

    UpdateHub updateHub; // signal hub, to avoid n:m connections
    Clipboard clipboard; // must be global to all windows
    UserManual userManual;
    ColorScheme colorscheme;
    if (colorscheme.isDevelopment())
        colorscheme.dumpHeaderFile();

    QString initialFilename;
    if (argc > 1)
        initialFilename = argv[1];

    CableColorizer cableColorizer;
    PatchEditEngine thePatch;
    MainWindow mainWindow(&thePatch, initialFilename);
    mainWindow.show();
    return app.exec();
}
