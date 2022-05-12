#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "droidfirmware.h"
#include "patchview.h"
#include "rackview.h"
#include "undohistory.h"
#include "patchparser.h"

#include <QMainWindow>
#include <QToolBar>

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
    QString initialFilename;
    Patch *patch;
    QString filename; // of loaded patch
    RackView rackview;
    PatchView patchview;
    QToolBar *toolbar;
    QAction *undoAction;
    QAction *redoAction;
    QAction *addJackAction;
    QAction *editValueAction;
    QAction *newCircuitAction;
    QAction *editCircuitCommentAction;
    QAction *renamePatchSectionAction;
    QAction *addPatchSectionAction;
    QAction *deletePatchSectionAction;

    QMenu *fileMenu;

public:
    MainWindow(const QString &initialFilename);
    ~MainWindow();
    void setPatch(Patch *);

    void createFileMenu();
    void createEditMenu();
    void loadPatch(QString filename);
    Patch *getPatch() { return patch; };
    void registerEdit(QString name);
    void updateActions();

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);

private slots:
    void slotLoadPatch(const QString &filename);
    void newPatch();
    void open();
    void save();
    void undo();
    void redo();

signals:
    void sigStarted();

private:
    void createActions();
    bool checkModified();
    void loadFile(const QString &filename);
    QIcon icon(QString what) const;
};
#endif // MAINWINDOW_H
