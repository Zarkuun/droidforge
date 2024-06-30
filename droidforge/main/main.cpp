#include "globals.h"
#include "mainwindow.h"
#include "cablecolorizer.h"
#include "clipboard.h"
#include "colorscheme.h"
#include "usermanual.h"
#include "iconbase.h"
#include "windowlist.h"
#include "patchgeneratorbase.h"
#include "tuning.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

// http://www.mardy.it/blog/2019/10/implementing-open-with-on-macos-with-qt.html

MainWindow *the_mainwindow = 0;

class MyApplication : public QApplication
{
public:
    MyApplication(int &argc, char **argv)
        : QApplication(argc, argv) { }

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            if (the_mainwindow)
                the_mainwindow->theOperator()->openFileFromExternal(openEvent->file());
        }

        return QApplication::event(event);
    }
};


int main(int argc, char *argv[])
{
    // QApplication app(argc, argv);
    MyApplication app(argc, argv);
    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationVersion(APPLICATION_VERSION);
    app.setApplicationDisplayName(APPLICATION_NAME);
    app.setOrganizationName("Der Mann mit der Maschine");
    app.setOrganizationDomain("dmmdm.de");
    app.setWindowIcon(QIcon(":images/droidforge-icon.png"));

    // Global variable objects that are shared by all MainWindow
    DroidFirmware firmware;
    WindowList windowList;
    Clipboard clipboard; // must be global to all windows
    UserManual userManual;
    IconBase iconBase;
    CableColorizer cableColorizer;
    ColorScheme colorscheme;
    QSettings settings;

    if (colorscheme.isDevelopment())
        colorscheme.dumpHeaderFile();

    // Create and change to user's patch directory
    QDir dir = QDir::homePath();
    if (!dir.cd(PATCH_DIRECTORY_NAME)) {
        dir.mkdir(PATCH_DIRECTORY_NAME);
    }
    dir.cd(PATCH_DIRECTORY_NAME);
    QDir::setCurrent(dir.absolutePath());

    // Patch generators
    if (!dir.cd(PATCH_GENERATORS_SUBDIR))
        dir.mkdir(PATCH_GENERATORS_SUBDIR);
    dir.cd(PATCH_GENERATORS_SUBDIR);
    PatchGeneratorBase pgBase(dir);

    if (settings.value("patch_generators_enabled", false).toBool())
    {
        // If this setting is true, the generators had been enabled successfully
        // previously. If it fails now, we better disable them.
        if (!pgBase.enableGenerators())
            settings.setValue("patch_generators_enabled", false);
    }

    // Preflight check .. maybe move this to a function some day
    QString version = the_firmware->version();
    if (!QFile(QString(":firmware/droid-%1.fw").arg(version)).exists()
       || !QFile(QString(":firmware/master18-%1.fw").arg(version)).exists())
    {
        QMessageBox::warning(
            0,
            TR("Firmware file missing"),
            TR("The master firmware files for the version %1 are not included "
               "in this version of the Forge. You cannot do a firmware upgrade "
               "of the master. Sorry.").arg(version));
    }


    // Open same file as last time - or the one give on the command line.
    QString initialFilename;
    if (argc > 1)
        initialFilename = argv[1];
    else if (settings.contains("lastfile"))
        initialFilename = settings.value("lastfile").toString();

    for (unsigned n=1; n < MAX_UNTITLED_BACKUPS; n++) {
        QString path = PatchOperator::untitledBackupPath(n);
        QFile file(path);
        if (file.exists()) {
            QMessageBox box(
                QMessageBox::Warning,
                TR("Backup of untitled patch detected!"),
                TR("There is a backup file of a patch that has never been saved, "
                   "and the Forge seems to have crashed or was killed otherwise. "
                   "Do you want to save that backup into a real patch file? "),
                QMessageBox::Ok | QMessageBox::Cancel);

            if (box.exec() == QMessageBox::Ok)
            {
                QString newFilePath = QFileDialog::getSaveFileName(
                    0,
                    TR("Save patch to file"),
                    "",
                    TR("DROID patch files (*.ini)"));

                if (!newFilePath.isEmpty()) {
                    file.remove(newFilePath); // rename does not do this
                    file.rename(newFilePath);
                    // open the restored file instead of the default one.
                    initialFilename = newFilePath;
                }
            }
        }
    }


    MainWindow *mainWindow = new MainWindow(initialFilename);
    the_mainwindow = mainWindow;
    mainWindow->show();

#ifdef QT_DEBUG
    if (!firmware.checkAllDescriptions())
        exit(1);
#endif

    return app.exec();
}
