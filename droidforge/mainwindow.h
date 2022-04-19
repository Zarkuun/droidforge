#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "droidfirmware.h"
#include "patchview.h"
#include "rackview.h"
#include "undohistory.h"
#include "patchparser.h"

#include <QMainWindow>

class Patch;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow;
extern MainWindow *the_forge;

extern DroidFirmware *the_firmware;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    DroidFirmware firmware;
    UndoHistory undoHistory;
    PatchParser parser;
    Patch *patch;
    QString filename; // of loaded patch
    RackView rackview;

    PatchView patchview;
    QAction *undoAction;
    QAction *redoAction;

public:
    MainWindow();
    ~MainWindow();
    void setPatch(Patch *);

    void createFileMenu();
    void createEditMenu();
    bool loadPatch(QString filename);
    Patch *getPatch() { return patch; };
    void registerEdit(QString name);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void open();
    void save();
    void undo();
    void redo();

private:
    void createActions();
    void updateActions();
    bool maybeSave();
    void loadFile(const QString &filename);
};
#endif // MAINWINDOW_H
