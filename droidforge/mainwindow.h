#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "droidfirmware.h"
#include "patchview.h"
#include "rackview.h"
#include "undohistory.h"
#include "patchparser.h"

#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>

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
    QSplitter *splitter;
    QToolBar *toolbar;
    QAction *undoAction;
    QAction *redoAction;
    QAction *addJackAction;
    QAction *editValueAction;
    QAction *newCircuitAction;
    QAction *openEnclosingFolderAction;
    QAction *editCircuitCommentAction;
    QAction *renamePatchSectionAction;
    QAction *addPatchSectionAction;
    QAction *deletePatchSectionAction;
    QAction *addControllerAction;

    QMenu *fileMenu;

public:
    MainWindow(const QString &initialFilename);
    ~MainWindow();
    void setPatch(Patch *);
    void loadPatch(QString filename);
    Patch *getPatch() { return patch; };
    void registerEdit(QString name);
    void patchHasChanged();

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);

private:
    void createFileMenu();
    void createRecentFileActions();
    void createEditMenu();
    void createRackMenu();
    void createActions();
    bool checkModified();
    QIcon icon(QString what) const;
    QStringList getRecentFiles();
    void addToRecentFiles(const QString &path);
    void openDirInFinder(const QString &filename);
    void updateActions();
    void updateWindowTitle();
    void updateRackView();

private slots:
    void loadFile(const QString &filename);
    void newPatch();
    void open();
    void save();
    void saveAs();
    void openEnclosingFolder();
    void undo();
    void redo();
    void splitterMoved();

signals:
    void sigStarted();

};
#endif // MAINWINDOW_H
