#include "mainwindow.h"
#include "droidforge.h"
#include "patchparser.h"
#include "patch.h"
#include "rack.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    DroidForge forge;
    QApplication a(argc, argv);

    Patch patch;
    PatchParser parser;
    QMessageBox box;

    if (!parser.parse("/Users/mk/git/droidforge/testpatch.ini", &patch))
    {
        box.setText("Fehler!");
        box.exec();
        QApplication::quit();
    }

    Rack rack(patch);

    MainWindow w(&rack, &patch);
    w.resize(800,1000);
    w.show();
    return a.exec();
}
