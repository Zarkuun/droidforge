#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editoractions.h"
#include "droidfirmware.h"
#include "patchsectionmanager.h"
#include "patchsectionview.h"
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

class MainWindow : public QMainWindow
{
    Q_OBJECT
    VersionedPatch *patch; // is never 0!!!
    EditorActions editorActions;
    DroidFirmware firmware;
    QStatusBar *statusbar;
    RackView rackView;
    PatchSectionView patchSectionView;
    PatchSectionManager patchSectionManager;

    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    unsigned currentProblem;
    PatchParser parser;
    QString initialFilename;
    QString filePath; // of loaded patch
    QSplitter *rackSplitter;
    QSplitter *sectionSplitter;

    QToolBar *toolbar;

public:
    MainWindow(const QString &initialFilename);
    ~MainWindow();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    Patch *getPatch() { return patch; }; // TODO????
    void registerEdit(QString name);
    void patchHasChanged();
    void hiliteRegisters(const RegisterList &registers);
    void clickOnRegister(AtomRegister);
    QIcon icon(QString what) const;
    QDir userPatchDirectory() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);

private:
    void createFileMenu();
    void createRecentFileActions(QMenu *);
    void createEditMenu();
    void createViewMenu();
    void createRackMenu();
    void createToolbar();
    void createMenus();
    void connectActions();
    bool checkModified();
    QStringList getRecentFiles();
    void addToRecentFiles(const QString &path);
    void openDirInFinder(const QString &filename);
    void updateWindowTitle();
    void repaintPatchView();
    void createStatusBar();
    bool interactivelyRemapRegisters(Patch *otherpatch);

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
    void editProperties();


signals:
    void patchChanged(VersionedPatch *); // all pointers are invalid
    void patchModified();

    void sigStarted();
    void problemsChanged(unsigned);

};
#endif // MAINWINDOW_H
