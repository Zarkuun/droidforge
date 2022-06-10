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
    MainWindow(PatchEditEngine *patch, QString initialFilename);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent* event);
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);

private:
    void createMenus();
    void createFileMenu();
    void createEditMenu();
    void createSectionMenu();
    void createViewMenu();
    void createRackMenu();
    void createToolbar();
    void createStatusBar();
    void updateWindowTitle();
    void updateStatusbarMessage();

private slots:
    void modifyPatch();
    void cursorMoved();
    void splitterMoved();

signals:
    void allActionsAborted();
};
#endif // MAINWINDOW_H
