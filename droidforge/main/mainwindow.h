#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cablestatusindicator.h"
#include "patchproblemindicator.h"
#include "droidfirmware.h"
#include "patchview.h"
#include "rackview.h"
#include "versionedpatch.h"
#include "patchparser.h"

#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QDir>
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

class VersionedPatch;

class MainWindow;
extern MainWindow *the_forge;

extern DroidFirmware *the_firmware;

#define FILE_MODE_LOAD 0
#define FILE_MODE_INTEGRATE 1

typedef enum {
    ACTION_ADD_CONTROLLER,
    ACTION_ADD_JACK,
    ACTION_EDIT_VALUE,
    ACTION_START_PATCHING,
    ACTION_FINISH_PATCHING,
    ACTION_ABORT_PATCHING,
    ACTION_FOLLOW_INTERNAL_CABLE,
    ACTION_RENAME_INTERNAL_CABLE,
    ACTION_NEW_CIRCUIT,
    ACTION_EDIT_CIRCUIT_COMMENT,
    ACTION_MOVE_INTO_SECTION,
    ACTION_JUMP_TO_NEXT_PROBLEM,
    NUM_ACTIONS,
} action_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    CableStatusIndicator *cableStatusIndicator;
    PatchProblemIndicator *patchProblemIndicator;
    unsigned currentProblem;
    QAction *actions[NUM_ACTIONS];

private:
    DroidFirmware firmware;
    PatchParser parser;
    QString initialFilename;
    VersionedPatch *patch;
    QString filePath; // of loaded patch
    RackView rackView;
    PatchView patchView;
    QSplitter *splitter;
    QToolBar *toolbar;

    QMenu *editMenu;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionPasteSmart;
    QAction *actionAddJack;
    QAction *actionOpenEnclosingFolder;
    QAction *actionRenamePatchSection;
    QAction *actionNewPatchSection;
    QAction *actionDeletePatchSection;

    QMenu *fileMenu;
    // TODO: Umstellen auf actions[]
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionExportSelection;
    QAction *actionIntegrate;

    QMenu *viewMenu;
    QAction *actionResetZoom;
    QAction *actionZoomIn;
    QAction *actionZoomOut;

public:
    MainWindow(const QString &initialFilename);
    ~MainWindow();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    Patch *getPatch() { return patch; };
    void registerEdit(QString name);
    void patchHasChanged();
    void updateActions();
    void updateClipboardInfo(QString info);
    void hiliteRegisters(const RegisterList &registers);
    void clickOnRegister(AtomRegister);
    QIcon icon(QString what) const;
    QDir userPatchDirectory() const;
    CableStatusIndicator *cableIndicator() { return cableStatusIndicator; };
    QAction *action(action_t a) { return actions[a]; };

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
    void updateWindowTitle();
    void updateRackView();
    void repaintPatchView();
    void createStatusBar();

private slots:
    void loadFile(const QString &filename, int how);
    void newPatch();
    void open();
    void integrate();
    void save();
    void saveAs();
    void exportSelection();
    void openEnclosingFolder();
    void undo();
    void redo();
    void jumpToNextProblem();
    void splitterMoved();
    void cursorMoved(int section, const CursorPosition &pos);

signals:
    void sigStarted();
    void patchChanged();
    void problemsChanged(unsigned);

};
#endif // MAINWINDOW_H
