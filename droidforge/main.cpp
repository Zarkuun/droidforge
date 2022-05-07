#include "parseexception.h"
#include "mainwindow.h"
#include "patch.h"

#include "unistd.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Der Mann mit der Maschine");
    QCoreApplication::setOrganizationDomain("dmmdm.de");
    QCoreApplication::setApplicationName("Droid Forge");

    QApplication a(argc, argv);
    QFile cssFile(":droidforge.css");
    if (!cssFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot load style sheet.";
    }
    else {
        QString css(cssFile.readAll());
        a.setStyleSheet(css);
        cssFile.close();
    }

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
