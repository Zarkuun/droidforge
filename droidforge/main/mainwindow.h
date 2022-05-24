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
#include <QDir>
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

class Patch;

class MainWindow;
extern MainWindow *the_forge;

extern DroidFirmware *the_firmware;

#define FILE_MODE_LOAD 0
#define FILE_MODE_INTEGRATE 1

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QStatusBar *statusbar;

private:
    DroidFirmware firmware;
    UndoHistory undoHistory;
    PatchParser parser;
    QString initialFilename;
    Patch *patch;
    QString filename; // of loaded patch
    RackView rackView;
    PatchView patchView;
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

    QMenu *viewMenu;
    QAction *zoomResetAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;

public:
    MainWindow(const QString &initialFilename);
    ~MainWindow();
    void setPatch(Patch *);
    void loadPatch(const QString &filename);
    void integratePatch(const QString &filename);
    Patch *getPatch() { return patch; };
    void registerEdit(QString name);
    void patchHasChanged();
    void hiliteRegisters(const RegisterList &registers);
    void clickOnRegister(AtomRegister);
    QIcon icon(QString what) const;
    QAction *getAddControllerAction() { return addControllerAction; };
    QDir userPatchDirectory() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);

private:
    void debug();
    void createFileMenu();
    void createRecentFileActions();
    void createEditMenu();
    void createViewMenu();
    void createRackMenu();
    void createActions();
    bool checkModified();
    QStringList getRecentFiles();
    void addToRecentFiles(const QString &path);
    void openDirInFinder(const QString &filename);
    void updateActions();
    void updateWindowTitle();
    void updateRackView();
    void repaintPatchView();

private slots:
    void loadFile(const QString &filename, int how);
    void newPatch();
    void open();
    void integrate();
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
