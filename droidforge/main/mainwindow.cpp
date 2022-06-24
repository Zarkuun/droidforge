#include "globals.h"
#include "mainwindow.h"
#include "parseexception.h"
#include "patch.h"
#include "rackview.h"
#include "modulebuilder.h"
#include "patchparser.h"
#include "tuning.h"
#include "os.h"
#include "updatehub.h"
#include "patchsectionview.h"
#include "patchview.h"
#include "colorscheme.h"
#include "patchoperator.h"
#include "usermanual.h"

#include <QTextEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QtGlobal>
#include <QDesktopServices>

MainWindow *the_forge;

MainWindow::MainWindow(PatchEditEngine *patch, QString initialFilename)
    : QMainWindow()
    , PatchView(patch)
    , editorActions(patch)
    , patchOperator(patch, initialFilename)
    , rackView(patch)
    , patchSectionView(patch)
    , patchSectionManager(patch)
    , cableStatusIndicator(patch)
    , patchProblemIndicator(patch)
{
    the_forge = this;

    setWindowTitle(APPLICATION_NAME);
    QIcon appIcon(":droidforge.icns");
    setWindowIcon(appIcon);

    menubar = new QMenuBar(this);
    setMenuBar(menubar);

    rackSplitter = new QSplitter(this);
    setCentralWidget(rackSplitter);
    rackSplitter->setOrientation(Qt::Vertical);
    rackSplitter->setHandleWidth(RACV_SPLITTER_HANDLE_WIDTH);

    sectionSplitter = new QSplitter(rackSplitter);
    sectionSplitter->setOrientation(Qt::Horizontal);
    sectionSplitter->addWidget(&patchSectionManager);
    sectionSplitter->addWidget(&patchSectionView);
    sectionSplitter->setStretchFactor(0, 0);
    sectionSplitter->setStretchFactor(1, 1);

    rackSplitter->addWidget(&rackView);
    rackSplitter->addWidget(sectionSplitter);

    resize(800, 600); // TODO
    QSettings settings;
    if (settings.contains("mainwindow/position")) {
        move(settings.value("mainwindow/position").toPoint());
        resize(settings.value("mainwindow/size").toSize());
    }
    if (settings.contains("mainwindow/splitposition"))
        rackSplitter->restoreState(settings.value("mainwindow/splitposition").toByteArray());
    connect(rackSplitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

    CONNECT_ACTION(ACTION_ABOUT, &MainWindow::about);
    CONNECT_ACTION(ACTION_LICENSE, &MainWindow::showLicense);
    CONNECT_ACTION(ACTION_RACK_ZOOM_IN, &MainWindow::rackZoomIn);
    CONNECT_ACTION(ACTION_RACK_ZOOM_OUT, &MainWindow::rackZoomOut);
    CONNECT_ACTION(ACTION_RACK_RESET_ZOOM, &MainWindow::rackZoomReset);
    CONNECT_ACTION(ACTION_USER_MANUAL, &MainWindow::showUserManual);
    CONNECT_ACTION(ACTION_CIRCUIT_MANUAL, &MainWindow::showCircuitManual);

    createMenus();
    createToolbar();
    createStatusBar();

    // Events that we are interested in
    connect(the_hub, &UpdateHub::patchModified, this, &MainWindow::modifyPatch);
    connect(the_hub, &UpdateHub::sectionSwitched, this, &MainWindow::cursorMoved);
    connect(the_hub, &UpdateHub::cursorMoved, this, &MainWindow::cursorMoved);

    // Some special connections that do not deal with update events
    connect(&rackView, &RackView::registerClicked, &patchSectionView, &PatchSectionView::clickOnRegister);
    connect(the_colorscheme, &ColorScheme::changed, the_hub, &UpdateHub::patchModified);
}

MainWindow::~MainWindow()
{
    delete patch;
}
void MainWindow::modifyPatch()
{
    QFileInfo fi(patch->getFilePath());
    QString patchName = fi.baseName();
    if (patchName == "")
        patchName = tr("(untitled)");
    QString title = patchName + " - " + APPLICATION_NAME;
    if (patch->isModified())
        title += tr(" (modified)");
    setWindowTitle(title);
    cursorMoved();
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!patchSectionView.handleKeyPress(event))
        event->ignore();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    patchOperator.quit();
    event->ignore();
}
void MainWindow::resizeEvent(QResizeEvent *)
{
    QSettings settings;
    settings.setValue("mainwindow/size", size());
}
void MainWindow::moveEvent(QMoveEvent *)
{
    QSettings settings;
    settings.setValue("mainwindow/position", pos());
}
void MainWindow::createMenus()
{
    createFileMenu();
    createRackMenu();
    createEditMenu();
    createSectionMenu();
    createViewMenu();
    createHelpMenu();

    // Add actions to the main window that have no menu entry
    // or toolbar, so that the keyboard shortcuts will work.
    ADD_ACTION(ACTION_NEXT_SECTION, this);
    ADD_ACTION(ACTION_PREVIOUS_SECTION, this);
    ADD_ACTION(ACTION_ABORT_ALL_ACTIONS, this);
}
void MainWindow::createFileMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&File"));

    ADD_ACTION(ACTION_NEW, menu);
    ADD_ACTION(ACTION_OPEN, menu);
    patchOperator.createRecentFileActions(menu);
    ADD_ACTION(ACTION_INTEGRATE_PATCH, menu);
    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    ADD_ACTION(ACTION_OPEN_ENCLOSING_FOLDER, menu);
    #endif

    menu->addSeparator();

    ADD_ACTION(ACTION_SAVE, menu);
    ADD_ACTION(ACTION_SAVE_AS, menu);
    ADD_ACTION(ACTION_EXPORT_SELECTION, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_PATCH_PROPERTIES, menu);
    ADD_ACTION(ACTION_EDIT_PATCH_SOURCE, menu);
    if (the_colorscheme->isDevelopment())
        ADD_ACTION(ACTION_CONFIGURE_COLORS, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_JUMP_TO_NEXT_PROBLEM, menu);
    ADD_ACTION(ACTION_UPLOAD_TO_DROID, menu);
    ADD_ACTION(ACTION_SAVE_TO_SD, menu);
    ADD_ACTION(ACTION_QUIT, menu);


}
void MainWindow::createEditMenu()
{
    QMenu *menu = menuBar()->addMenu(ZERO_WIDTH_SPACE + tr("&Edit"));

    ADD_ACTION(ACTION_UNDO, menu);
    ADD_ACTION(ACTION_REDO, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_CUT, menu);
    ADD_ACTION(ACTION_COPY, menu);
    ADD_ACTION(ACTION_PASTE, menu);
    ADD_ACTION(ACTION_PASTE_SMART, menu);
    ADD_ACTION(ACTION_SELECT_ALL, menu);
    ADD_ACTION(ACTION_DISABLE, menu);
    ADD_ACTION(ACTION_ENABLE, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_MOVE_CIRCUIT_UP, menu);
    ADD_ACTION(ACTION_MOVE_CIRCUIT_DOWN, menu);
    ADD_ACTION(ACTION_SORT_JACKS, menu);
    ADD_ACTION(ACTION_EXPAND_ARRAY, menu);
    ADD_ACTION(ACTION_EXPAND_ARRAY_MAX, menu);
    ADD_ACTION(ACTION_ADD_MISSING_JACKS, menu);
    ADD_ACTION(ACTION_REMOVE_UNDEFINED_JACKS, menu);
    ADD_ACTION(ACTION_FIX_LED_MISMATCH, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_NEW_CIRCUIT, menu);
    ADD_ACTION(ACTION_ADD_JACK, menu);
    ADD_ACTION(ACTION_EDIT_VALUE, menu);
    ADD_ACTION(ACTION_FOLLOW_CABLE, menu);
    ADD_ACTION(ACTION_RENAME_CABLE, menu);
    ADD_ACTION(ACTION_START_PATCHING, menu);
    ADD_ACTION(ACTION_FINISH_PATCHING, menu);
    ADD_ACTION(ACTION_ABORT_PATCHING, menu);
    ADD_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, menu);
    ADD_ACTION(ACTION_EDIT_JACK_COMMENT, menu);
    ADD_ACTION(ACTION_EDIT_CIRCUIT_SOURCE, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);
}
void MainWindow::createSectionMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("Section"));
    ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_PASTE_AS_SECTION, menu);
    ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_MERGE_WITH_PREVIOUS_SECTION, menu);
    ADD_ACTION(ACTION_MERGE_WITH_NEXT_SECTION, menu);
    ADD_ACTION(ACTION_MERGE_ALL_SECTIONS, menu);
    ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_EDIT_SECTION_SOURCE, menu);
}
void MainWindow::createViewMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&View"));
    ADD_ACTION(ACTION_RESET_ZOOM, menu);
    ADD_ACTION(ACTION_ZOOM_IN, menu);
    ADD_ACTION(ACTION_ZOOM_OUT, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_FOLD_UNFOLD, menu);
    ADD_ACTION(ACTION_FOLD_UNFOLD_ALL, menu);

    menu->addSeparator(); // separates "Enter full screen" on Mac
}
void MainWindow::createRackMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&Rack"));
    ADD_ACTION(ACTION_ADD_CONTROLLER, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_RACK_RESET_ZOOM, menu);
    ADD_ACTION(ACTION_RACK_ZOOM_IN, menu);
    ADD_ACTION(ACTION_RACK_ZOOM_OUT, menu);
}
void MainWindow::createHelpMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&Help"));
    ADD_ACTION(ACTION_ABOUT, menu);
    ADD_ACTION(ACTION_USER_MANUAL, menu);
    ADD_ACTION(ACTION_CIRCUIT_MANUAL, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_LICENSE, menu);
}
void MainWindow::createStatusBar()
{
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);
    statusbar->addPermanentWidget(&cableStatusIndicator);
    statusbar->addPermanentWidget(&patchProblemIndicator);
    statusbar->addPermanentWidget(&clipboardIndicator);
}
void MainWindow::createToolbar()
{
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    ADD_ACTION(ACTION_TOOLBAR_NEW, toolbar);
    ADD_ACTION(ACTION_TOOLBAR_OPEN, toolbar);
    ADD_ACTION(ACTION_TOOLBAR_SAVE, toolbar);
    toolbar->addSeparator();
    ADD_ACTION(ACTION_TOOLBAR_NEW_CIRCUIT, toolbar);
    ADD_ACTION(ACTION_TOOLBAR_ADD_JACK, toolbar);
    ADD_ACTION(ACTION_TOOLBAR_ADD_CONTROLLER, toolbar);
    toolbar->addSeparator();
    ADD_ACTION(ACTION_TOOLBAR_PROBLEMS, toolbar);
    ADD_ACTION(ACTION_TOOLBAR_UPLOAD_TO_DROID, toolbar);
    ADD_ACTION(ACTION_TOOLBAR_SAVE_TO_SD, toolbar);
}
void MainWindow::splitterMoved()
{
    QSettings settings;
    settings.setValue("mainwindow/splitposition", rackSplitter->saveState());
}
void MainWindow::rackZoomIn()
{
    rackZoom(1);
}
void MainWindow::rackZoomOut()
{
    rackZoom(-1);
}
void MainWindow::rackZoomReset()
{
    rackZoom(0);
}
void MainWindow::showUserManual()
{
    the_manual->show();
}
void MainWindow::showCircuitManual()
{
    const Circuit *circuit = section()->currentCircuit();
    if (circuit)
        the_manual->showCircuit(circuit->getName());
}
void MainWindow::updateWindowTitle()
{
    // TODO: Mit signal ansetuern
    QString title;
    if (filePath.isEmpty())
        title = tr("(untitled patch)") + " - " + tr(APPLICATION_NAME);
    else
        title = filePath + " - " + tr(APPLICATION_NAME);
    if (patch->isModified())
        title += " (" + tr("modified") + ")";
    setWindowTitle(title);
}
void MainWindow::updateStatusbarMessage()
{
    QStringList infos;

    int sectionNr = patch->currentSectionIndex();
    auto pos = section()->cursorPosition();

    // Problems
    QString problem = patch->problemAt(sectionNr, pos);
    if (problem != "")
        infos.append(problem);

    // Jack comments
    if (pos.column == 0) // Information
    {
        auto ja = section()->currentJackAssignment();
        if (ja && ja->getComment() != "") {
            infos.append(ja->getComment());
        }
    }

    // Labels of registers
    const Atom *atom = patch->currentAtom();
    if (atom && atom->isRegister()) {
        AtomRegister *ar = (AtomRegister *)atom;
        RegisterLabel label = patch->registerLabel(*ar);
        if (label.shorthand != "")
            infos.append(label.shorthand);
        if (label.description != "")
            infos.append(label.description);
    }

    QString message = infos.join(", ");
    if (message != "")
        statusbar->showMessage(message);
    else
        statusbar->clearMessage();
}
void MainWindow::rackZoom(int whence)
{
    QList<int> currentSizes = rackSplitter->sizes();
    int totalsize = currentSizes[0] + currentSizes[1];
    if (whence == 0)
        currentSizes[0] = RACV_NORMAL_HEIGHT;
    else if (whence == 1) {
        currentSizes[0] *= 1.2;
        if (currentSizes[0] < RACV_MIN_HEIGHT)
            currentSizes[0] = RACV_MIN_HEIGHT;
    }
    else {
        currentSizes[0] /= 1.2;
        if (currentSizes[0] < RACV_MIN_HEIGHT)
            currentSizes[0] = 0;
    }
    currentSizes[1] = totalsize - currentSizes[0];
    rackSplitter->setSizes(currentSizes);
}

void MainWindow::about()
{
    QString firmware_version = the_firmware->version();
    QMessageBox::about( this,
                        tr("About DROID Forge"),
                        tr("DROID Forge version %1.\n\n"

                           "This version of DROID Forge assumes that your "
                           "DROID master is running firmware version %2 or newer.\n\n"

                           "Copyright Mathias Kettner 2022.\n\n"

                           "This program is free software: you can redistribute it and/or modify it under "
                           "the terms of the GNU General Public License as published by the Free Software "
                           "Foundation, either version 3 of the License, or (at your option) any later version."
                           "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; "
                           "without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
                           "See the GNU General Public License for more details.\n\n"

                           "You should have received a copy of the GNU General Public License along with this program. "
                           "If not, see <https://www.gnu.org/licenses/>. ")
                        .arg(APPLICATION_VERSION, firmware_version));
}

void MainWindow::showLicense()
{
    QDesktopServices::openUrl(QUrl(LICENSE_URL));
}
void MainWindow::cursorMoved()
{
    updateStatusbarMessage();

    // Make sure that after an undo the cursor is at the correct
    // position - that is the last position before the actual
    // edit was done. We do that by tracking all non-editing
    // cursor moves and update the last patch version in the
    // commit history.
    patch->commitCursorPosition();
}
