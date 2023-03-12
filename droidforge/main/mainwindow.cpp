#include "globals.h"
#include "iconbase.h"
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
#include "preferencesdialog.h"
#include "windowlist.h"

#include <QTextEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSettings>
#include <QtGlobal>
#include <QDesktopServices>

#define mainWindow this // make ACTION() macros work

MainWindow::MainWindow(QString initialFilename, const Patch *initialRack)
    : QMainWindow()
    , PatchView(&thePatch)
    , bringToFrontAction("solala", this)
    , editorActions(this, &thePatch)
    , patchOperator(this, &thePatch, initialFilename, initialRack)
    , rackView(this, &thePatch)
    , patchSectionView(this, &thePatch)
    , patchSectionManager(this, &thePatch)
    , patchSizeIndicator(this, &thePatch)
    , cableStatusIndicator(this, &thePatch)
    , patchProblemIndicator(this, &thePatch)
    , clipboardIndicator(this)
    , moduleBuilder(this)
    , currentStatusDump(0)
{
    setWindowTitle(APPLICATION_NAME);
    QIcon appIcon(":droidforge.icns");
    setWindowIcon(appIcon);

    bringToFrontAction.setCheckable(true);

    menubar = new QMenuBar(this);
    setMenuBar(menubar);

    rackSplitter = new QSplitter(this);
    setCentralWidget(rackSplitter);
    rackSplitter->setOrientation(Qt::Vertical);
    rackSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);

    sectionSplitter = new QSplitter(rackSplitter);
    sectionSplitter->setOrientation(Qt::Horizontal);
    sectionSplitter->addWidget(&patchSectionManager);
    sectionSplitter->addWidget(&patchSectionView);
    sectionSplitter->setStretchFactor(0, 0);
    sectionSplitter->setStretchFactor(1, 1);
    sectionSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);

    rackSplitter->addWidget(&rackView);
    rackSplitter->addWidget(sectionSplitter);

    QSettings settings;
    if (settings.contains("mainwindow/size"))
        resize(settings.value("mainwindow/size").toSize());
    else
        resize(MAIN_WIDTH, MAIN_HEIGHT);

    // Make sure a new windows does not open at the same position an
    // existing one does.
    if (the_windowlist->count() > 0)
        move(the_windowlist->newPosition());

    connect(rackSplitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);
    connect(the_windowlist, &WindowList::changed, this, &MainWindow::updateWindowMenu);
    connect(&findPanel, &FindPanel::keyCaptured, &patchSectionView, &PatchSectionView::keyCaptured);

    CONNECT_ACTION(ACTION_ABOUT, &MainWindow::about);
    CONNECT_ACTION(ACTION_LICENSE, &MainWindow::showLicense);
    CONNECT_ACTION(ACTION_PREFERENCES, &MainWindow::editPreferences);
    CONNECT_ACTION(ACTION_RACK_ZOOM_IN, &MainWindow::rackZoomIn);
    CONNECT_ACTION(ACTION_RACK_ZOOM_OUT, &MainWindow::rackZoomOut);
    CONNECT_ACTION(ACTION_RACK_RESET_ZOOM, &MainWindow::rackZoomReset);
    CONNECT_ACTION(ACTION_USER_MANUAL, &MainWindow::showUserManual);
    CONNECT_ACTION(ACTION_DISCORD, &MainWindow::showDiscord);
    CONNECT_ACTION(ACTION_CIRCUIT_MANUAL, &MainWindow::showCircuitManual);
    CONNECT_ACTION(ACTION_CLEAR_SETTINGS, &MainWindow::clearSettings);
    CONNECT_ACTION(ACTION_FIND, &MainWindow::showFindPanel);
    CONNECT_ACTION(ACTION_ABORT_ALL_ACTIONS, &MainWindow::abortAllActions);
    CONNECT_ACTION(ACTION_MINIMIZE_WINDOW, &MainWindow::showMinimized);
    CONNECT_ACTION(ACTION_NEXT_WINDOW, &MainWindow::nextWindow);
    CONNECT_ACTION(ACTION_PREVIOUS_WINDOW, &MainWindow::previousWindow);

    connect(&bringToFrontAction, &QAction::triggered, this, &MainWindow::bringToFront);

    createMenus();
    createToolbar();
    createStatusBar();

    // Detect application palette changs
    installEventFilter(this);

    // Events that we are interested in
    connect(theHub(), &UpdateHub::patchModified, this, &MainWindow::modifyPatch);
    connect(theHub(), &UpdateHub::sectionSwitched, this, &MainWindow::cursorMoved);
    connect(theHub(), &UpdateHub::cursorMoved, this, &MainWindow::cursorMoved);

    // Event that we create
    connect(this, &MainWindow::patchModified, theHub(), &UpdateHub::modifyPatch);

    // Some special connections that do not deal with update events
    connect(&rackView, &RackView::registerClicked, &patchSectionView, &PatchSectionView::clickOnRegister);
    connect(the_colorscheme, &ColorScheme::changed, theHub(), &UpdateHub::patchModified);

    the_windowlist->add(this);
}
MainWindow::~MainWindow()
{
    the_windowlist->remove(this);
}
bool MainWindow::searchActive() const
{
    return findPanel.isEnabled();
}
void MainWindow::setStatusbarText(QString text)
{
    statusbarText->setText(text);
}
void MainWindow::addStatusDumpsMenu(QMenu *menu)
{
    fileMenu->addMenu(menu);
}
void MainWindow::showStatusDump(const StatusDump *dump)
{
    currentStatusDump = dump;
    emit patchModified();
}
QString MainWindow::patchTitle() const
{
    QString title = patch->getTitle();
    if (title != "")
        return title;
    else {
        QString filepath = patch->getFilePath();
        if (filepath != "")
            return QFileInfo(filepath).baseName();
        else
            return tr("untitled");
    }
}
const QString &MainWindow::getFilePath() const
{
    return patch->getFilePath();
}
void MainWindow::bringToFront()
{
    raise();
    activateWindow();
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
}
void MainWindow::hideFindPanel()
{
    findPanel.setDisabled(true);
    findPanel.hide();
}
void MainWindow::saveMeLikeAll()
{
    patchOperator.save();
}
void MainWindow::modifyPatch()
{
    QString title = patchTitle() + " - " + APPLICATION_NAME;
    if (patch->isModified())
        title += tr(" (modified)");
    setWindowTitle(title);
    updateBTFAction();
    cursorMoved();
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!patchSectionView.handleKeyPress(event))
        event->ignore();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!patchOperator.checkModified())
        event->ignore();
    else {
        event->accept();
        delete this;
    }
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
void MainWindow::showEvent(QShowEvent *)
{
    QSettings settings;
    if (settings.contains("mainwindow/splitposition")) {
        rackSplitter->restoreState(settings.value("mainwindow/splitposition").toByteArray());
        rackSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);
    }
    else {
        rackZoomReset();
    }
}
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        the_colorscheme->darkLightSwitch();
        editorActions.updateIcons();
        emit patchModified();
    }
    else if (event->type() == QEvent::ActivationChange) {
        bringToFrontAction.setChecked(isActiveWindow());
    }
    return QObject::eventFilter(obj, event);
}
void MainWindow::createMenus()
{
    createFileMenu();
    createEditMenu();
    createSectionMenu();
    createViewMenu();
    createHelpMenu();
    createWindowsMenu();

    // Add actions to the main window that have no menu entry
    // or toolbar, so that the keyboard shortcuts will work.
    ADD_ACTION(ACTION_NEXT_SECTION, this);
    ADD_ACTION(ACTION_PREVIOUS_SECTION, this);
    ADD_ACTION(ACTION_ABORT_ALL_ACTIONS, this);
}
void MainWindow::createFileMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    ADD_ACTION(ACTION_NEW, fileMenu);
    ADD_ACTION(ACTION_NEW_WITH_SAME_RACK, fileMenu);

    ADD_ACTION(ACTION_OPEN, fileMenu);
    recentFilesMenu = fileMenu->addMenu(tr("Open &recent patch"));
    patchOperator.createRecentFileActions(recentFilesMenu);

    fileMenu->addSeparator();

    ADD_ACTION(ACTION_INTEGRATE_PATCH, fileMenu);
    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    ADD_ACTION(ACTION_OPEN_ENCLOSING_FOLDER, fileMenu);
    #endif

    ADD_ACTION(ACTION_SAVE, fileMenu);
    ADD_ACTION(ACTION_SAVE_AS, fileMenu);
    ADD_ACTION(ACTION_SAVE_ALL, fileMenu);
    ADD_ACTION(ACTION_EXPORT_SELECTION, fileMenu);
    ADD_ACTION(ACTION_CLOSE_WINDOW, fileMenu);

    fileMenu->addSeparator();

    ADD_ACTION(ACTION_PATCH_PROPERTIES, fileMenu);
    ADD_ACTION(ACTION_PREFERENCES, fileMenu);
    ADD_ACTION(ACTION_EDIT_PATCH_SOURCE, fileMenu);
    ADD_ACTION(ACTION_BARE_PATCH_SOURCE, fileMenu);
    if (the_colorscheme->isDevelopment())
        ADD_ACTION(ACTION_CONFIGURE_COLORS, fileMenu);

    fileMenu->addSeparator();

    ADD_ACTION(ACTION_JUMP_TO_NEXT_PROBLEM, fileMenu);
    ADD_ACTION(ACTION_UPLOAD_TO_DROID, fileMenu);
    ADD_ACTION(ACTION_SAVE_TO_SD, fileMenu);
    ADD_ACTION(ACTION_CLEAR_SETTINGS, fileMenu);
    fileMenu->addMenu(patchOperator.statusDumpsMenu());
    ADD_ACTION(ACTION_QUIT, fileMenu);
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
    ADD_ACTION(ACTION_PASTE_SMARTLY, menu);
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
    ADD_ACTION(ACTION_NEW_JACK, menu);
    ADD_ACTION(ACTION_NEW_CONTROLLER, menu);
    ADD_ACTION(ACTION_EDIT_VALUE, menu);
    ADD_ACTION(ACTION_FOLLOW_CABLE, menu);
    ADD_ACTION(ACTION_RENAME_CABLE, menu);
    ADD_ACTION(ACTION_REWRITE_CABLE_NAMES, menu);
    ADD_ACTION(ACTION_FIND, menu);
    ADD_ACTION(ACTION_SET_BOOKMARK, menu);
    ADD_ACTION(ACTION_JUMP_TO_BOOKMARK, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_START_PATCHING, menu);
    ADD_ACTION(ACTION_FINISH_PATCHING, menu);
    ADD_ACTION(ACTION_ABORT_PATCHING, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_FOLLOW_REGISTER, menu);
    ADD_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, menu);
    ADD_ACTION(ACTION_EDIT_JACK_COMMENT, menu);
    ADD_ACTION(ACTION_EDIT_LABEL, menu);
    ADD_ACTION(ACTION_EDIT_CIRCUIT_SOURCE, menu);
}
void MainWindow::createSectionMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&Section"));
    ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_SAVE_SECTION, menu);
    ADD_ACTION(ACTION_PASTE_AS_SECTION, menu);
    ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION_SMARTLY, menu);
    ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_MERGE_WITH_PREVIOUS_SECTION, menu);
    ADD_ACTION(ACTION_MERGE_WITH_NEXT_SECTION, menu);
    ADD_ACTION(ACTION_MERGE_ALL_SECTIONS, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_PREVIOUS_SECTION, menu);
    ADD_ACTION(ACTION_NEXT_SECTION, menu);
    ADD_ACTION(ACTION_MOVE_SECTION_UP, menu);
    ADD_ACTION(ACTION_MOVE_SECTION_DOWN, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_EDIT_SECTION_COMMENT, menu);
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

    menu->addSeparator();

    ADD_ACTION(ACTION_SHOW_REGISTER_LABELS, menu);
    ADD_ACTION(ACTION_SHOW_REGISTER_USAGE, menu);
    ADD_ACTION(ACTION_TEXT_MODE, menu);
    ADD_ACTION(ACTION_SHOW_G8_ON_DEMAND, menu);
    ADD_ACTION(ACTION_SHOW_X7_ON_DEMAND, menu);
    ADD_ACTION(ACTION_RIGHT_TO_LEFT, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_RACK_RESET_ZOOM, menu);
    ADD_ACTION(ACTION_RACK_ZOOM_IN, menu);
    ADD_ACTION(ACTION_RACK_ZOOM_OUT, menu);

    menu->addSeparator(); // separates "Enter full screen" on Mac
}
void MainWindow::createHelpMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&Help"));
    ADD_ACTION(ACTION_ABOUT, menu);
    ADD_ACTION(ACTION_USER_MANUAL, menu);
    ADD_ACTION(ACTION_CIRCUIT_MANUAL, menu);
    ADD_ACTION(ACTION_DISCORD, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_LICENSE, menu);
}
void MainWindow::createWindowsMenu()
{
    windowMenu = menuBar()->addMenu(tr("&Window"));
    updateWindowMenu();
}
void MainWindow::createStatusBar()
{
    statusbar = new QStatusBar(this);
    statusbarText = new QLabel("Das ist die Meldung");
    statusbarIcon = new QLabel("i");
    statusbarIcon->setStyleSheet("QLabel { margin-left: 5px; }");
    setStatusBar(statusbar);
    statusbar->addPermanentWidget(statusbarIcon);
    statusbar->addPermanentWidget(statusbarText, 1);
    statusbar->addPermanentWidget(&cableStatusIndicator);
    statusbar->addPermanentWidget(&patchSizeIndicator);
    statusbar->addPermanentWidget(&patchProblemIndicator);
    statusbar->addPermanentWidget(&clipboardIndicator);
    findPanel.setDisabled(true);
    findPanel.hide();
    statusbar->addPermanentWidget(&findPanel);
    connect(&findPanel, &FindPanel::search, &patchOperator, &PatchOperator::search);
    connect(&patchOperator, &PatchOperator::searchStatsChanged, &findPanel, &FindPanel::updateSearchStats);
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
void MainWindow::saveSplitPosition()
{
    QSettings settings;
    settings.setValue("mainwindow/splitposition", rackSplitter->saveState());
}
void MainWindow::splitterMoved()
{
    saveSplitPosition();
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
void MainWindow::clearSettings()
{
    QMessageBox box(
                QMessageBox::Warning,
                tr("Clear settings!"),
                tr("Do you really want to clear all implicit settings like the "
                   "sizes of dialog windows, your list of recent files, the "
                   "zoom level and other similar things?"),
                QMessageBox::Ok | QMessageBox::Cancel,
                this);
    if (box.exec() == QMessageBox::Ok) {
        QSettings settings;
        settings.clear();
        resize(MAIN_WIDTH, MAIN_HEIGHT);
        QList<int> oldSizes = sectionSplitter->sizes();
        int delta = PSM_NORMAL_WIDTH - oldSizes[0];
        QList<int> newSizes = { oldSizes[0] + delta, oldSizes[1] - delta };
        sectionSplitter->setSizes(newSizes);
        patchSectionView.clearSettings();
        rackZoomReset();
        patchOperator.clearRecentFiles();
    }
}
void MainWindow::showFindPanel()
{
    findPanel.setEnabled(true);
    findPanel.show();
}
void MainWindow::abortAllActions()
{
    findPanel.hide();
    findPanel.setEnabled(false);
}
void MainWindow::nextWindow()
{
    the_windowlist->nextWindow(this)->bringToFront();
}
void MainWindow::previousWindow()
{
    the_windowlist->previousWindow(this)->bringToFront();
}
void MainWindow::updateStatusbarMessage()
{
    QStringList infos;
    QPixmap icon;

    int sectionNr = patch->currentSectionIndex();
    auto pos = section()->cursorPosition();

    // Problems
    QString problem = patch->problemAt(sectionNr, pos);
    if (problem != "") {
        infos.append(problem);
        icon = PIXMAP("problemsmall");
    }

    // Jack comments
    if (pos.column == 0) // Information
    {
        auto ja = section()->currentJackAssignment();
        if (ja && ja->getComment() != "") {
            infos.append(ja->getComment());
            icon = PIXMAP("info");
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
    if (message != "") {
        if (!icon.isNull())
            statusbarIcon->setPixmap(icon.scaledToHeight(statusbar->height() - 10, Qt::SmoothTransformation));
        else
            statusbarIcon->setPixmap(icon);
        statusbarText->setText(message);
    }
    else {
        statusbarText->clear();
        statusbarIcon->clear();
    }
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
    saveSplitPosition();
}

void MainWindow::updateBTFAction()
{
    QString fileName = getFilePath();
    QString patchName = patchTitle();
    QString title;
    if (fileName != "")
        title = patchName + " - " + fileName;
    else
        title = patchName;
    if (patch->isModified())
        title += " (" + tr("modified") + ")";
    bringToFrontAction.setText(title);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    ADD_ACTION(ACTION_NEW_WINDOW, windowMenu);
    ADD_ACTION(ACTION_NEW_WINDOW_WITH_SAME_RACK, windowMenu);
    ADD_ACTION(ACTION_OPEN_IN_NEW_WINDOW, windowMenu);

    windowMenu->addSeparator();

    ADD_ACTION(ACTION_MINIMIZE_WINDOW, windowMenu);
    ADD_ACTION(ACTION_NEXT_WINDOW, windowMenu);
    ADD_ACTION(ACTION_PREVIOUS_WINDOW, windowMenu);
    windowMenu->addSeparator();
    the_windowlist->addMenuEntries(windowMenu);
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
void MainWindow::editPreferences()
{
    PreferencesDialog::editPreferences();
    patch->updateProblems();
    emit patchModified(); // Update patch size usage and potential problems
}
void MainWindow::showDiscord()
{
    QDesktopServices::openUrl(QUrl(DISCORD_URL));
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
