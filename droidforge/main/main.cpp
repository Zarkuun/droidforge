#include "globals.h"
#include "mainwindow.h"
#include "cablecolorizer.h"
#include "clipboard.h"
#include "colorscheme.h"
#include "usermanual.h"
#include "iconbase.h"
#include "windowlist.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

// http://www.mardy.it/blog/2019/10/implementing-open-with-on-macos-with-qt.html

MainWindow *hirn_mainwindow = 0;

class MyApplication : public QApplication
{
public:
    MyApplication(int &argc, char **argv)
        : QApplication(argc, argv) { }

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            if (hirn_mainwindow)
                hirn_mainwindow->theOperator()->openFileFromExternal(openEvent->file());
            // QMessageBox::about( 0,
            //             tr("Man will eine Datei Laden"),
            //             openEvent->file());
            // qDebug() << "Open file" << openEvent->file();
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
    if (colorscheme.isDevelopment())
        colorscheme.dumpHeaderFile();

    // Create and change to user's patch directory
    QDir dir = QDir::homePath();
    if (!dir.cd(PATCH_DIRECTORY_NAME)) {
        dir.mkdir(PATCH_DIRECTORY_NAME);
    }
    dir.cd(PATCH_DIRECTORY_NAME);
    QDir::setCurrent(dir.absolutePath());

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

                if (!newFilePath.isEmpty())
                    file.rename(newFilePath);
            }
        }
    }


    QString initialFilename;
    QSettings settings;
    if (argc > 1)
        initialFilename = argv[1];
    else if (settings.contains("lastfile"))
        initialFilename = settings.value("lastfile").toString();
    MainWindow *mainWindow = new MainWindow(initialFilename);
    hirn_mainwindow = mainWindow;
    mainWindow->show();

#ifdef QT_DEBUG
    if (!firmware.checkAllDescriptions())
        exit(1);
#endif

    return app.exec();
}
