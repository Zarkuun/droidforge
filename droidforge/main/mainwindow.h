#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editoractions.h"
#include "droidfirmware.h"
#include "patchsectionmanager.h"
#include "patchsectionview.h"
#include "rackview.h"
#include "patcheditengine.h"
#include "patchparser.h"
#include "patchoperator.h"
#include "clipboardindicator.h"
#include "cablestatusindicator.h"
#include "patchproblemindicator.h"

#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QDir>
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>

class PatchEditEngine;

class MainWindow;
extern MainWindow *the_forge;
extern DroidFirmware *the_firmware;

#define FILE_MODE_LOAD 0
#define FILE_MODE_INTEGRATE 1

class MainWindow : public QMainWindow, PatchOperator
{
    Q_OBJECT

    DroidFirmware firmware;

    // all PatchOperators
    EditorActions editorActions;
    RackView rackView;
    PatchSectionView patchSectionView;
    PatchSectionManager patchSectionManager;
    CableStatusIndicator cableStatusIndicator;
    PatchProblemIndicator patchProblemIndicator;
    ClipboardIndicator clipboardIndicator;

    QStatusBar *statusbar;
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    PatchParser parser;
    QString initialFilename;
    QString filePath; // of loaded patch
    QSplitter *rackSplitter;
    QSplitter *sectionSplitter;

    QToolBar *toolbar;

public:
    MainWindow(PatchEditEngine *patch, const QString &initialFilename);
    ~MainWindow();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    void clickOnRegister(AtomRegister);
    QIcon icon(QString what) const;
    QDir userPatchDirectory() const;

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);

private:
    void createFileMenu();
    void createRecentFileActions(QMenu *);
    void createMenus();
    void createEditMenu();
    void createSectionMenu();
    void createViewMenu();
    void createRackMenu();
    void createToolbar();
    void connectActions();
    bool checkModified();
    QStringList getRecentFiles();
    void addToRecentFiles(const QString &path);
    void openDirInFinder(const QString &filename);
    void updateWindowTitle();
    void repaintPatchView();
    void createStatusBar();

private slots:
    void cursorMoved();
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
    void splitterMoved();
    void editProperties();

signals:
    void patchModified();

    void sigStarted();
    void problemsChanged(unsigned);

};
#endif // MAINWINDOW_H
