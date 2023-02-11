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
#include "memoryindicator.h"
#include "cablestatusindicator.h"
#include "patchproblemindicator.h"
#include "findpanel.h"

#include <QMainWindow>
#include <QToolBar>
#include <QSplitter>
#include <QDir>
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QLabel>

class PatchEditEngine;

class MainWindow;
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
    MemoryIndicator memoryIndicator;
    CableStatusIndicator cableStatusIndicator;
    PatchProblemIndicator patchProblemIndicator;
    ClipboardIndicator clipboardIndicator;
    FindPanel findPanel;

    QStatusBar *statusbar;
    QLabel *statusbarText;
    QLabel *statusbarIcon;
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QMenu *recentFilesMenu;
    QString initialFilename;
    QString filePath; // of loaded patch
    QSplitter *rackSplitter;
    QSplitter *sectionSplitter;

    QToolBar *toolbar;

public:
    MainWindow(PatchEditEngine *patch, QString initialFilename);
    ~MainWindow();
    // TODO: This is so sick
    bool searchActive() const;
    void setStatusbarText(QString text);
    PatchOperator *theOperator() { return &patchOperator; };

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent *) override;
    void moveEvent(QMoveEvent *) override;
    void showEvent(QShowEvent *) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void createMenus();
    void createFileMenu();
    void createEditMenu();
    void createSectionMenu();
    void createViewMenu();
    void createHelpMenu();
    void createToolbar();
    void createStatusBar();
    void updateStatusbarMessage();
    void rackZoom(int whence);

private slots:
    void about();
    void editPreferences();
    void showDiscord();
    void showLicense();
    void modifyPatch();
    void cursorMoved();
    void splitterMoved();
    void rackZoomIn();
    void rackZoomOut();
    void rackZoomReset();
    void showUserManual();
    void showCircuitManual();
    void clearSettings();
    void showFindPanel();
    void abortAllActions();

signals:
    void allActionsAborted();
    void patchModified();
};
#endif // MAINWINDOW_H
