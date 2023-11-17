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
#include "patchsizeindicator.h"
#include "cablestatusindicator.h"
#include "patchproblemindicator.h"
#include "findpanel.h"
#include "modulebuilder.h"
#include "updatehub.h"
#include "patchgenerator.h"

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
class StatusDump;

class MainWindow;

#define FILE_MODE_LOAD 0
#define FILE_MODE_INTEGRATE 1

class MainWindow : public QMainWindow, PatchView
{
    Q_OBJECT

    PatchEditEngine thePatch;
    UpdateHub updateHub;
    QAction bringToFrontAction;

    // all PatchViews
    EditorActions editorActions;
    PatchOperator patchOperator;
    RackView rackView;
    PatchSectionView patchSectionView;
    PatchSectionManager patchSectionManager;
    PatchSizeIndicator patchSizeIndicator;
    CableStatusIndicator cableStatusIndicator;
    PatchProblemIndicator patchProblemIndicator;
    ClipboardIndicator clipboardIndicator;

    ModuleBuilder moduleBuilder;
    FindPanel findPanel;
    QStatusBar *statusbar;
    QLabel *statusbarText;
    QLabel *statusbarIcon;
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    Dialog *verticalLayout;
    QMenuBar *menubar;
    QMenu *fileMenu;
    QMenu *recentFilesMenu;
    QMenu *generatorsMenu;
    QMenu *windowMenu;
    QString initialFilename;
    QString filePath; // of loaded patch
    QSplitter *rackSplitter;
    QSplitter *sectionSplitter;
    QToolBar *toolbar;

    const StatusDump *currentStatusDump;

public:
    MainWindow(QString initialFilename, const Patch *initialRack=0);
    ~MainWindow();
    // TODO: This is so sick
    bool searchActive() const;
    void setStatusbarText(QString text);
    void addStatusDumpsMenu(QMenu *menu);
    const StatusDump *statusDump() { return currentStatusDump; };
    void showStatusDump(const StatusDump *dump);
    PatchOperator *theOperator() { return &patchOperator; };
    EditorActions *theActions() { return &editorActions; };
    ModuleBuilder *theModuleBuilder() { return &moduleBuilder; };
    UpdateHub *theHub() { return &updateHub; };
    QString patchTitle() const;
    const QString &getFilePath() const;
    void bringToFront();
    void hideFindPanel();
    void saveMeLikeAll();
    QAction *btfAction() { return &bringToFrontAction; };
    void updateGeneratorsShortcut(int index);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent *) override;
    void moveEvent(QMoveEvent *) override;
    void showEvent(QShowEvent *) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    // Das hier geht nicht. Wir bekommen drag aber kein drop
    // void dragEnterEvent(QDragEnterEvent *event) override;
    // void dragMoveEvent(QDragMoveEvent *event) override;
    // void dropEvent(QDropEvent *event) override;

private:
    void createMenus();
    void createFileMenu();
    void createRackMenu();
    void populateGeneratorsMenu();
    void createEditMenu();
    void createSectionMenu();
    void createViewMenu();
    void createHelpMenu();
    void createVideoActions(QMenu *menu);
    void createWindowsMenu();
    void createToolbar();
    void createStatusBar();
    void updateStatusbarMessage();
    void rackZoom(int whence);
    void updateBTFAction();
    void saveSplitPosition();

private slots:
    void updateWindowMenu();
    void about();
    void editPreferences();
    void showDiscord();
    void showVideo(QString youtube_code);
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
    void nextWindow();
    void previousWindow();

signals:
    void allActionsAborted();
    void patchModified();
};
#endif // MAINWINDOW_H
