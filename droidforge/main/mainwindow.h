#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "editoractions.h"
#include "droidfirmware.h"
#include "patchoperator.h"
#include "patchsectionmanager.h"
#include "patchsectionview.h"
#include "rackview.h"
#include "patcheditengine.h"
#include "patchview.h"
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

class MainWindow : public QMainWindow, PatchView
{
    Q_OBJECT

    DroidFirmware firmware;

    // all PatchViews
    EditorActions editorActions;
    PatchOperator patchOperator;
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
    QString initialFilename;
    QString filePath; // of loaded patch
    QSplitter *rackSplitter;
    QSplitter *sectionSplitter;

    QToolBar *toolbar;

public:
    MainWindow(PatchEditEngine *patch, const QString &initialFilename);
    ~MainWindow();
    void clickOnRegister(AtomRegister);
    QIcon icon(QString what) const;
    QDir userPatchDirectory() const;
    // TODO: Warum hier? Soll irgendwie weg

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);
    void resizeEvent(QResizeEvent *);

private:
    void createFileMenu();
    void createMenus();
    void createEditMenu();
    void createSectionMenu();
    void createViewMenu();
    void createRackMenu();
    void createToolbar();
    void connectActions();
    void openDirInFinder(const QString &filename);
    void updateWindowTitle();
    void repaintPatchView();
    void createStatusBar();
    void updateStatusbarMessage();

private slots:
    void modifyPatch();
    void cursorMoved();
    void openEnclosingFolder();
    void splitterMoved();
    void configureColors();

signals:

    void sigStarted();
    void problemsChanged(unsigned);

};
#endif // MAINWINDOW_H
