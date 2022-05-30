#include "mainwindow.h"
#include "parseexception.h"
#include "patch.h"
#include "rackview.h"
#include "modulebuilder.h"
#include "patchparser.h"
#include "patchview.h"
#include "tuning.h"
#include "os.h"


#include <QTextEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QSettings>
#include <QtGlobal>
#include <QProcess>

MainWindow *the_forge;
DroidFirmware *the_firmware;

MainWindow::MainWindow(const QString &initialFilename)
    : QMainWindow()
    , initialFilename(initialFilename)
    , patch(0)
{
    the_forge = this;
    the_firmware = &firmware;

    setWindowTitle(APPLICATION_NAME);

    menubar = new QMenuBar(this);
    setMenuBar(menubar);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    setCentralWidget(splitter);
    splitter->addWidget(&rackView);
    splitter->addWidget(&patchView);
    splitter->setHandleWidth(RACV_SPLITTER_HANDLE_WIDTH);
    connect(&patchView, &PatchView::cursorMoved, this, &MainWindow::cursorMoved);

    resize(800, 600);
    QSettings settings;
    if (settings.contains("mainwindow/splitposition"))
        splitter->restoreState(settings.value("mainwindow/splitposition").toByteArray());
    connect(splitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // TODO: Actions und Menus getrennt regeln
    createActions();
    createStatusBar();

    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [&] () {loadFile(initialFilename, FILE_MODE_LOAD);});
}


void MainWindow::createStatusBar()
{
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    cableStatusIndicator = new CableStatusIndicator;
    statusbar->addPermanentWidget(cableStatusIndicator);

    patchProblemIndicator = new PatchProblemIndicator;
    statusbar->addPermanentWidget(patchProblemIndicator);
    connect(this, &MainWindow::problemsChanged, patchProblemIndicator, &PatchProblemIndicator::problemsChanged);
    connect(patchProblemIndicator, &PatchProblemIndicator::clicked, actions[ACTION_JUMP_TO_NEXT_PROBLEM], &QAction::trigger);
}

void MainWindow::debug()
{
}

MainWindow::~MainWindow()
{
    if (patch)
        delete patch;
}

void MainWindow::loadPatch(const QString &aFilename)
{
    Patch newpatch;
    parser.parse(aFilename, &newpatch);
    setPatch(newpatch.clone());

    filename = aFilename;
    undoHistory.reset(&newpatch);
    the_forge->patchHasChanged();
}

void MainWindow::integratePatch(const QString &aFilename)
{
    Patch otherpatch;
    parser.parse(aFilename, &otherpatch);
    Patch *newPatch = patchView.integratePatch(&otherpatch);
    if (newPatch) {
        registerEdit(tr("integrating other patch '%1'").arg(otherpatch.getTitle()));
        setPatch(newPatch);
        the_forge->patchHasChanged();
    }
}

void MainWindow::registerEdit(QString name)
{
    undoHistory.snapshot(patch, name);
    the_forge->patchHasChanged();
}

void MainWindow::setPatch(Patch *newpatch)
{
    if (patch)
        delete patch;
    patch = newpatch;
    patch->updateProblems();
    rackView.setPatch(patch);
    patchView.setPatch(patch);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        patchView.abortAllActions();
        // rackView.abortAllAction();
    }
    else  if (!patchView.handleKeyPress(event)) {
        event->ignore();
    }
}


void MainWindow::closeEvent(QCloseEvent *)
{
    // TODO: Das hier wieder aktivieren
    // if (!checkModified()) {
    //     event->ignore();
    //     return;
    // }

    QSettings settings;
    settings.setValue("mainwindow/position", pos());
    settings.setValue("mainwindow/size", size());
}

void MainWindow::loadFile(const QString &filename, int how)
{
    if (FILE_MODE_LOAD && !checkModified())
        return;

    try {
        addToRecentFiles(filename);
        if (how == FILE_MODE_LOAD) {
            loadPatch(filename);
            // integratePatch("/Users/mk/git/droidforge/hirn.ini");
            // exit(0);
        }
        else
            integratePatch(filename);

    }
    catch (ParseException &e) {
        QMessageBox box;
        box.setText(MainWindow::tr("Cannot load ") + filename);
        box.setInformativeText(e.toString());
        box.setStandardButtons(QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Critical);
        // TODO: Size of message box?
        // box.setBaseSize(QSize(600, 220));
        box.exec();
        QApplication::quit();
    }
}

void MainWindow::createActions()
{
    createFileMenu();
    createRackMenu();
    createEditMenu();
    createViewMenu();

    QAction *nextSectionAct = new QAction(tr("Next section"));
    nextSectionAct->setShortcut(QKeySequence(tr("Ctrl+Right")));
    nextSectionAct->setStatusTip(tr("Switch to the next section"));
    addAction(nextSectionAct);
    connect(nextSectionAct, &QAction::triggered, &patchView, &PatchView::nextSection);

    QAction *prevSectionAct = new QAction(tr("Previous section"));
    prevSectionAct->setShortcut(QKeySequence(tr("Ctrl+Left")));
    prevSectionAct->setStatusTip(tr("Switch to the previous section"));
    addAction(prevSectionAct);
    connect(prevSectionAct, &QAction::triggered, &patchView, &PatchView::previousSection);

    for (unsigned i=0; i<NUM_ACTIONS; i++)
        actions[i]->setShortcutVisibleInContextMenu(true);
}


void MainWindow::patchHasChanged()
{
    updateProblems();
    patchView.abortPatching();
    updateActions();
    updateWindowTitle();
    updateRackView();
    repaintPatchView();
    emit patchChanged();
}

void MainWindow::hiliteRegisters(const RegisterList &registers)
{
    rackView.hiliteRegisters(registers);
}

void MainWindow::clickOnRegister(AtomRegister ar)
{
    patchView.clickOnRegister(ar);
}

void MainWindow::updateActions()
{
    // File menu
    actionSave->setEnabled(undoHistory.isModified());
    actionExportSelection->setEnabled(patchView.circuitsSelected());
    actionOpenEnclosingFolder->setEnabled(!filename.isEmpty());

    // Edit menu
    if (undoHistory.undoPossible()) {
        actionUndo->setText(tr("&Undo ") + undoHistory.nextUndoTitle());
        actionUndo->setEnabled(true);
    }
    else {
        actionUndo->setText(tr("&Undo"));
        actionUndo->setEnabled(false);
    }

    if (undoHistory.redoPossible()) {
        actionRedo->setText(tr("&Redo ") + undoHistory.nextRedoTitle());
        actionRedo->setEnabled(true);
    }
    else {
        actionRedo->setText(tr("&Redo"));
        actionRedo->setEnabled(false);
    }

    actionPaste->setEnabled(patchView.clipboardFilled());
    actionPasteSmart->setEnabled(patchView.circuitsInClipboard());

    const PatchSectionView *psv = patchView.currentPatchSectionView();
    bool empty = !psv || psv->isEmpty();
    actions[ACTION_ADD_JACK]->setEnabled(!empty);
    actions[ACTION_EDIT_VALUE]->setEnabled(!empty);
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setEnabled(!empty);
    const Atom *atom = 0;
    if (psv)
        atom = psv->currentAtom();
    bool isAtAtom = psv && psv->getCursorPosition().isAtAtom();

    actions[ACTION_FOLLOW_INTERNAL_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_RENAME_INTERNAL_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_START_PATCHING]->setVisible(isAtAtom && !patchView.isPatching());
    actions[ACTION_FINISH_PATCHING]->setVisible(isAtAtom && patchView.isPatching());
    actions[ACTION_ABORT_PATCHING]->setVisible(patchView.isPatching());

    if (psv && patchView.numSections() > 1) {
        actionDeletePatchSection->setText(tr("Delete section '%1'").arg(psv->getTitle()));
        actionDeletePatchSection->setEnabled(true);
    }
    else {
        actionDeletePatchSection->setText(tr("Delete section"));
        actionDeletePatchSection->setEnabled(false);
    }
    actions[ACTION_MOVE_INTO_SECTION]->setEnabled(patchView.circuitsSelected());
}

void MainWindow::updateProblems()
{
    if (patch) {
        patch->updateProblems();
        actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setEnabled(patch->numProblems());
        currentProblem = 0;
        emit problemsChanged(patch->numProblems());
    }
}

void MainWindow::updateClipboardInfo(QString)
{
    // statusbar->showMessage(info); // TODO: brauchen wir das?
}

void MainWindow::updateWindowTitle()
{
    QString title;
    if (filename.isEmpty())
        title = tr("(untitled patch)") + " - " + tr("DROID Forge");
    else
        title = filename + " - " + tr("DROID Forge");
    if (undoHistory.isModified())
        title += " (" + tr("modified") + ")";
    setWindowTitle(title);
}


void MainWindow::updateRackView()
{
    rackView.setPatch(patch);
    patchView.updateRegisterHilites();
}

void MainWindow::repaintPatchView()
{
    PatchSectionView *psv = patchView.currentPatchSectionView();
    if (psv)
        psv->updateCircuits();
}

QDir MainWindow::userPatchDirectory() const
{
    // TODO: Make patch directory configurable
    QDir dir = QDir::homePath();
    if (!dir.cd(PATCH_DIRECTORY_NAME)) {
        dir.mkdir(PATCH_DIRECTORY_NAME);
        dir.cd(PATCH_DIRECTORY_NAME);
    }
    return dir;
}

void MainWindow::createFileMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    // New
    actionNew = new QAction(icon("settings_input_composite"), tr("&New..."), this);
    actionNew->setShortcut(QKeySequence(tr("Ctrl+Shift+Alt+N")));
    actionNew->setStatusTip(tr("Create a new patch from scratch"));
    connect(actionNew, &QAction::triggered, this, &MainWindow::newPatch);
    fileMenu->addAction(actionNew);
    toolbar->addAction(actionNew);

    // Open
    actionOpen = new QAction(icon("open_in_browser"), tr("&Open..."), this);
    actionOpen->setShortcuts(QKeySequence::Open);
    actionOpen->setStatusTip(tr("Open an existing patch"));
    connect(actionOpen, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(actionOpen);
    toolbar->addAction(actionOpen);

    // Save
    actionSave = new QAction(icon("save"), tr("&Save..."), this);
    actionSave->setShortcuts(QKeySequence::Save);
    actionSave->setStatusTip(tr("Save patch to file"));
    connect(actionSave, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(actionSave);
    toolbar->addAction(actionSave);

    // Save as...
    actionSaveAs = new QAction(tr("Save &as..."), this);
    actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    actionSaveAs->setStatusTip(tr("Save patch to a different file"));
    connect(actionSaveAs, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(actionSaveAs);

    // Export selection
    actionExportSelection = new QAction(tr("E&xport selection as patch..."), this);
    actionExportSelection->setStatusTip(tr("Save the currently selected circuits into a new patch file"));
    connect(actionExportSelection, &QAction::triggered, this, &MainWindow::exportSelection);
    fileMenu->addAction(actionExportSelection);

    // Open enclosing folder
#if (defined Q_OS_MACOS || defined Q_OS_WIN)
#ifdef Q_OS_MACOS
    QString title = tr("Reveal in finder");
#else
    QString title = tr("Re&veal in explorer");
#endif
    actionOpenEnclosingFolder = new QAction(title, this);
    actionOpenEnclosingFolder->setStatusTip(tr("Open the folder where the current patch is located."));
    connect(actionOpenEnclosingFolder, &QAction::triggered, this, &MainWindow::openEnclosingFolder);
    fileMenu->addAction(actionOpenEnclosingFolder);
#endif

    // Recent files
    createRecentFileActions();

    // Integrate
    actionIntegrate = new QAction(icon("extension"), tr("&Integrate other patch as new section"), this);
    actionIntegrate->setShortcut(QKeySequence(tr("Ctrl+I")));
    actionIntegrate->setStatusTip(tr("Load another patch, add that as a new section after the currently selected section "
                               "and try to move the controls, inputs and outputs of that patch to unused "
                               "jacks and controlls"));
    connect(actionIntegrate, &QAction::triggered, this, &MainWindow::integrate);
    fileMenu->addAction(actionIntegrate);


    // Find next problem
    actions[ACTION_JUMP_TO_NEXT_PROBLEM] = new QAction(icon("warning"), tr("&Jump to next problem"), this);
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setShortcut(QKeySequence(tr("F6")));
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setStatusTip(tr("Jump to the next problem in your patch. You "
                                             "need to fix all these problems before you can load "
                                             "the patch to your master."));
    connect(actions[ACTION_JUMP_TO_NEXT_PROBLEM], &QAction::triggered, this, &MainWindow::jumpToNextProblem);
    fileMenu->addAction(actions[ACTION_JUMP_TO_NEXT_PROBLEM]);


    // Quit (automatically goes to Mac menu on mac)
    QAction *quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit DROID Forge"));
    connect(quitAct, &QAction::triggered, this, &MainWindow::close);
    fileMenu->addAction(quitAct);

    fileMenu->addSeparator();

    // Patch properties
    QAction *patchPropertiesAct = new QAction(icon("dns"), tr("&Patch properties..."), this);
    patchPropertiesAct->setShortcut(QKeySequence(tr("Ctrl+.")));
    connect(patchPropertiesAct, &QAction::triggered, &patchView, &PatchView::editProperties);
    fileMenu->addAction(patchPropertiesAct);
}

void MainWindow::createRecentFileActions()
{
    QMenu *menu = fileMenu->addMenu(tr("Open recent file"));
    QStringList recentFiles = getRecentFiles();
    for (qsizetype i=0; i<recentFiles.count(); i++) {
        QFileInfo fi(recentFiles[i]);
        if (!fi.exists())
            continue;
        QAction *action = new QAction(fi.baseName(), this);
        QString path = fi.absoluteFilePath();
        connect(action, &QAction::triggered, this, [this, path]() { this->loadFile(path, FILE_MODE_LOAD); });
        menu->addAction(action);
    }
}

QStringList MainWindow::getRecentFiles()
{
    QSettings settings;
    return settings.value("recentfiles").toStringList();
}

void MainWindow::addToRecentFiles(const QString &path)
{
    QFileInfo fi(path);
    QStringList files = getRecentFiles();
    files.removeAll(fi.absoluteFilePath());
    files.prepend(fi.absoluteFilePath());
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();
    QSettings settings;
    settings.setValue("recentfiles", files);
}

void MainWindow::createEditMenu()
{
    editMenu = menuBar()->addMenu(ZERO_WIDTH_SPACE + tr("&Edit"));

    // Undo
    actionUndo = new QAction(icon("undo"), tr("&Undo"), this);
    actionUndo->setShortcuts(QKeySequence::Undo);
    actionUndo->setStatusTip(tr("Undo last edit action"));
    connect(actionUndo, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(actionUndo);

    // Redo
    actionRedo = new QAction(icon("redo"), tr("&Redo"), this);
    actionRedo->setShortcuts(QKeySequence::Redo);
    actionRedo->setStatusTip(tr("Redo last edit action"));
    connect(actionRedo, &QAction::triggered, this, &MainWindow::redo);
    editMenu->addAction(actionRedo);

    editMenu->addSeparator();

    // Cut
    actionCut = new QAction(icon("cut"), tr("C&ut"), this);
    actionCut->setShortcuts(QKeySequence::Cut);
    actionCut->setStatusTip(tr("Cut selection to clipboard"));
    connect(actionCut, &QAction::triggered, &patchView, &PatchView::cut);
    editMenu->addAction(actionCut);

    // Copy
    actionCopy = new QAction(icon("copy"), tr("&Copy"), this);
    actionCopy->setShortcuts(QKeySequence::Copy);
    actionCopy->setStatusTip(tr("Copy selected stuff to clipboard"));
    connect(actionCopy, &QAction::triggered, &patchView, &PatchView::copy);
    editMenu->addAction(actionCopy);

    // Paste
    actionPaste = new QAction(icon("paste"), tr("&Paste"), this);
    actionPaste->setShortcuts(QKeySequence::Paste);
    actionPaste->setStatusTip(tr("Paste contents from clipboard"));
    connect(actionPaste, &QAction::triggered, &patchView, &PatchView::paste);
    editMenu->addAction(actionPaste);

    // Paste smart
    actionPasteSmart = new QAction(tr("&Paste smart"), this);
    actionPasteSmart->setShortcut(QKeySequence(tr("Shift+Ctrl+V")));
    actionPasteSmart->setStatusTip(tr("Paste circuits from clipboard but remap registers and internal connections "
                                 "in order to avoid conflicts."));
    connect(actionPasteSmart, &QAction::triggered, &patchView, &PatchView::pasteSmart);
    editMenu->addAction(actionPasteSmart);

    // New circuit...
    actions[ACTION_NEW_CIRCUIT] = new QAction(icon("open_in_new"), tr("&New circuit..."), this);
    actions[ACTION_NEW_CIRCUIT]->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));
    connect(actions[ACTION_NEW_CIRCUIT], &QAction::triggered, &patchView, &PatchView::newCircuit);
    editMenu->addAction(actions[ACTION_NEW_CIRCUIT]);
    toolbar->addAction(actions[ACTION_NEW_CIRCUIT]);

    // New jacks assignment
    actions[ACTION_ADD_JACK] = new QAction(icon("settings_input_composite"), tr("&New jack..."), this);
    actions[ACTION_ADD_JACK]->setShortcut(QKeySequence(tr("Ctrl+N")));
    connect(actions[ACTION_ADD_JACK], &QAction::triggered, &patchView, &PatchView::addJack);
    editMenu->addAction(actions[ACTION_ADD_JACK]);
    toolbar->addAction(actions[ACTION_ADD_JACK]);

    // Edit current line / field
    actions[ACTION_EDIT_VALUE] = new QAction(icon("edit"), tr("&Edit element under cursor..."), this);
    actions[ACTION_EDIT_VALUE]->setShortcuts({ QKeySequence(tr("Enter")),
                                    QKeySequence(tr("Return"))});
    editMenu->addAction(actions[ACTION_EDIT_VALUE]);
    connect(actions[ACTION_EDIT_VALUE], &QAction::triggered, &patchView, &PatchView::editValue);

    // Follow internal cable
    actions[ACTION_FOLLOW_INTERNAL_CABLE] = new QAction(icon("youtube_searched_for"), tr("&Follow internal cable"), this);
    actions[ACTION_FOLLOW_INTERNAL_CABLE]->setShortcut(QKeySequence(tr("?")));
    editMenu->addAction(actions[ACTION_FOLLOW_INTERNAL_CABLE]);
    connect(actions[ACTION_FOLLOW_INTERNAL_CABLE], &QAction::triggered, &patchView, &PatchView::followInternalCable);

    // rename internal cable
    actions[ACTION_RENAME_INTERNAL_CABLE] = new QAction(tr("&Rename internal cable"), this);
    actions[ACTION_RENAME_INTERNAL_CABLE]->setShortcut(QKeySequence(tr("Ctrl+R")));
    editMenu->addAction(actions[ACTION_RENAME_INTERNAL_CABLE]);
    connect(actions[ACTION_RENAME_INTERNAL_CABLE], &QAction::triggered, &patchView, &PatchView::renameInternalCable);

    // Create internal cable
    actions[ACTION_START_PATCHING] = new QAction(icon("swap_horiz"), tr("Start creating internal cable"), this);
    actions[ACTION_START_PATCHING]->setShortcut(QKeySequence(tr("=")));
    editMenu->addAction(actions[ACTION_START_PATCHING]);
    connect(actions[ACTION_START_PATCHING], &QAction::triggered, &patchView, &PatchView::startPatching);

    // Finish internal connection
    actions[ACTION_FINISH_PATCHING] = new QAction(icon("swap_horiz"), tr("Finish creating internal cable"), this);
    actions[ACTION_FINISH_PATCHING]->setShortcut(QKeySequence(tr("=")));
    editMenu->addAction(actions[ACTION_FINISH_PATCHING]);
    connect(actions[ACTION_FINISH_PATCHING], &QAction::triggered, &patchView, &PatchView::finishPatching);

    // Abort patching
    actions[ACTION_ABORT_PATCHING] = new QAction(icon("swap_horiz"), tr("Abort creating internal cable"), this);
    editMenu->addAction(actions[ACTION_ABORT_PATCHING]);
    connect(actions[ACTION_ABORT_PATCHING], &QAction::triggered, &patchView, &PatchView::abortPatching);

    // Edit comment of current circuit
    actions[ACTION_EDIT_CIRCUIT_COMMENT] = new QAction(tr("Edit circuit comment..."), this);
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setShortcutVisibleInContextMenu(true);
    editMenu->addAction(actions[ACTION_EDIT_CIRCUIT_COMMENT]);
    connect(actions[ACTION_EDIT_CIRCUIT_COMMENT], &QAction::triggered, &patchView, &PatchView::editCircuitComment);

    editMenu->addSeparator();

    // Add section
    actionNewPatchSection = new QAction(tr("New section..."), this);
    editMenu->addAction(actionNewPatchSection);
    connect(actionNewPatchSection, &QAction::triggered, &patchView, &PatchView::newSectionAfterCurrent);

    // Rename section
    actionRenamePatchSection = new QAction(tr("Rename section..."), this);
    editMenu->addAction(actionRenamePatchSection);
    connect(actionRenamePatchSection, &QAction::triggered, &patchView, &PatchView::renameCurrentSection);

    // Delete section
    actionDeletePatchSection = new QAction(tr("Delete section"), this);
    editMenu->addAction(actionDeletePatchSection);
    connect(actionDeletePatchSection, &QAction::triggered, &patchView, &PatchView::deleteCurrentSection);

    // Move into sectin
    actions[ACTION_MOVE_INTO_SECTION] = new QAction(tr("Move selection into new section"), this);
    editMenu->addAction(actions[ACTION_MOVE_INTO_SECTION]);
    connect(actions[ACTION_MOVE_INTO_SECTION], &QAction::triggered, &patchView, &PatchView::moveIntoSection);
}

void MainWindow::createViewMenu()
{
    viewMenu = menuBar()->addMenu(tr("&View"));

    actionResetZoom = new QAction(icon("zoom_in"), tr("Normal font size"), this);
    actionResetZoom->setShortcut(QKeySequence(tr("Ctrl+0")));
    viewMenu->addAction(actionResetZoom);
    connect(actionResetZoom, &QAction::triggered, &patchView, [this] () { patchView.zoom(0); });

    actionZoomIn = new QAction(icon("zoom_in"), tr("Increase font size"), this);
    actionZoomIn->setShortcuts(QKeySequence::ZoomIn);
    viewMenu->addAction(actionZoomIn);
    connect(actionZoomIn, &QAction::triggered, &patchView, [this] () { patchView.zoom(1); });

    actionZoomOut = new QAction(icon("zoom_out"), tr("Outcrease font size"), this);
    actionZoomOut->setShortcuts(QKeySequence::ZoomOut);
    viewMenu->addAction(actionZoomOut);
    connect(actionZoomOut, &QAction::triggered, &patchView, [this] () { patchView.zoom(-1); });
}

void MainWindow::createRackMenu()
{
    QMenu *rackMenu = menuBar()->addMenu(tr("&Rack"));

    // Add controller
    actions[ACTION_ADD_CONTROLLER] = new QAction(icon("keyboard"), tr("&New controller..."), this);
    actions[ACTION_ADD_CONTROLLER]->setShortcut(QKeySequence(tr("Ctrl+Alt+N")));
    connect(actions[ACTION_ADD_CONTROLLER], &QAction::triggered, &rackView, &RackView::addController);
    rackMenu->addAction(actions[ACTION_ADD_CONTROLLER]);
    toolbar->addSeparator();
    toolbar->addAction(actions[ACTION_ADD_CONTROLLER]);
}

void MainWindow::newPatch()
{
    if (!checkModified())
        return;
    Patch newpatch;
    newpatch.addSection(new PatchSection(SECTION_DEFAULT_NAME));
    setPatch(newpatch.clone());
    undoHistory.reset(&newpatch);
    filename = "";
    the_forge->patchHasChanged();
}

void MainWindow::open()
{
    if (!checkModified())
        return;

    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName, FILE_MODE_LOAD);
}

void MainWindow::integrate()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName, FILE_MODE_INTEGRATE);
}

void MainWindow::save()
{
    if (filename.isEmpty())
        saveAs();
    else {
        patch->saveToFile(filename);
        undoHistory.clearModified();
        the_forge->patchHasChanged();
    }
}

void MainWindow::saveAs()
{
    QString newFilename = QFileDialog::getSaveFileName(
                this,
                tr("Save patch to new file"),
                filename,
                tr("DROID patch files (*.ini)"));
    if (!newFilename.isEmpty()) {
        patch->saveToFile(newFilename);
        filename = newFilename;
        undoHistory.clearModified();
        the_forge->patchHasChanged();
        addToRecentFiles(newFilename);
    }
}

void MainWindow::exportSelection()
{
    QString newFilename = QFileDialog::getSaveFileName(
                this,
                tr("Export selection into new patch"),
                "",
                tr("DROID patch files (*.ini)"));
    if (!newFilename.isEmpty()) {
        Patch *patch = patchView.getSelectionAsPatch();
        patch->saveToFile(newFilename);
        delete patch;
        addToRecentFiles(newFilename);
    }
}

void MainWindow::openEnclosingFolder()
{
    QFileInfo fileinfo(filename);
    openDirInFinder(fileinfo.absoluteFilePath());
}

void MainWindow::undo()
{
    if (undoHistory.undoPossible()) {
        if (patch)
            delete patch;
        patch = undoHistory.undo();
        patch->updateProblems();
        rackView.setPatch(patch);
        patchView.setPatch(patch);
        the_forge->patchHasChanged();
    }
}


void MainWindow::redo()
{
    if (undoHistory.redoPossible()) {
        if (patch)
            delete patch;
        patch = undoHistory.redo();
        patch->updateProblems();
        rackView.setPatch(patch);
        patchView.setPatch(patch);
        the_forge->patchHasChanged();
    }
}

void MainWindow::jumpToNextProblem()
{
    const PatchProblem *problem = patch->problem(currentProblem++);
    if (currentProblem >= patch->numProblems())
        currentProblem = 0;
    patchView.jumpTo(problem->getSection(), problem->getCursorPosition());
}


void MainWindow::splitterMoved()
{
    QSettings settings;
    settings.setValue("mainwindow/splitposition", splitter->saveState());
}

void MainWindow::cursorMoved(int section, const CursorPosition &pos)
{
    QString problem = patch->problemAt(section, pos);
    if (problem != "")
        statusbar->showMessage(problem);
    else
        statusbar->clearMessage();
}

bool MainWindow::checkModified()
{
    // TODO rackview modified
    if (undoHistory.isModified()) {
        QMessageBox box(
                    QMessageBox::Warning,
                    tr("Your patch is modified!"),
                    tr("Do you want to save your changes before you proceed?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    this);
        int ret = box.exec();
        switch (ret) {
        case QMessageBox::Save:
            // TODO: Check success of saving!
            save();
            return true;

        case QMessageBox::Discard:
            return true;

        default: // QMessageBox::Cancel:
            return false;
        }
    }
    else
        return true;
}


QIcon MainWindow::icon(QString what) const
{
    return QIcon(":/images/icons/white/" + what + ".png");
}

void MainWindow::openDirInFinder(const QString &filename)
{
#ifdef Q_OS_MACOS
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \""+filename+"\"";
    args << "-e";
    args << "end tell";
    QProcess::startDetached("osascript", args);

#endif
#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(filename);
    QProcess::startDetached("explorer", args);
#endif
}
