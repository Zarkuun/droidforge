#include "mainwindow.h"
#include "droidforge.h"
#include "patch.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DroidForge forge;

    if (!forge.loadPatch("/Users/mk/git/droidforge/testpatch.ini"))
    {
        QMessageBox box;
        box.setText("Fehler!");
        box.exec();
        QApplication::quit();
    }

    MainWindow w(forge.getPatch());
    w.resize(800,1000);
    w.move(1000, 0);
    w.show();
    return a.exec();
}
