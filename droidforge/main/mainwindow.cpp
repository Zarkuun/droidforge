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

    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    setCentralWidget(splitter);
    splitter->addWidget(&rackView);
    splitter->addWidget(&patchView);
    splitter->setHandleWidth(RACV_SPLITTER_HANDLE_WIDTH);

    resize(800, 600);
    QSettings settings;
    if (settings.contains("mainwindow/splitposition"))
        splitter->restoreState(settings.value("mainwindow/splitposition").toByteArray());
    connect(splitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    createActions();

    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [&] () {loadFile(initialFilename, FILE_MODE_LOAD);});
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
    patchHasChanged();
}

void MainWindow::integratePatch(const QString &aFilename)
{
    Patch otherpatch;
    parser.parse(aFilename, &otherpatch);
    Patch *newPatch = patchView.integratePatch(&otherpatch);
    if (newPatch) {
        registerEdit(tr("integrating other patch '%1'").arg(otherpatch.getTitle()));
        setPatch(newPatch);
        patchHasChanged();
    }
}

void MainWindow::registerEdit(QString name)
{
    undoHistory.snapshot(patch, name);
    patchHasChanged();
}

void MainWindow::setPatch(Patch *newpatch)
{
    if (patch)
        delete patch;
    patch = newpatch;
    rackView.setPatch(patch);
    patchView.setPatch(patch);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!patchView.handleKeyPress(event)) {
        event->ignore();
        // QWidget::keyPressEvent(event);
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!checkModified()) {
        event->ignore();
        return;
    }

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
    patchHasChanged();

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
}


void MainWindow::patchHasChanged()
{
    updateActions();
    updateWindowTitle();
    updateRackView();
    repaintPatchView();
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
    saveAct->setEnabled(undoHistory.isModified());
    exportSelectionAct->setEnabled(patchView.circuitsSelected());

    // Edit menu
    if (undoHistory.undoPossible()) {
        undoAction->setText(tr("&Undo ") + undoHistory.nextUndoTitle());
        undoAction->setEnabled(true);
    }
    else {
        undoAction->setText(tr("&Undo"));
        undoAction->setEnabled(false);
    }

    if (undoHistory.redoPossible()) {
        redoAction->setText(tr("&Redo ") + undoHistory.nextRedoTitle());
        redoAction->setEnabled(true);
    }
    else {
        redoAction->setText(tr("&Redo"));
        redoAction->setEnabled(false);
    }

    pasteAction->setEnabled(patchView.clipboardFilled());

    const PatchSectionView *psv = patchView.currentPatchSectionView();
    bool empty = !psv || psv->isEmpty();
    addJackAction->setEnabled(!empty);
    editValueAction->setEnabled(!empty);
    editCircuitCommentAction->setEnabled(!empty);
    if (patchView.numSections() > 1) {
        deletePatchSectionAction->setText(tr("Delete section") + " '" + psv->getTitle() + "'");
        deletePatchSectionAction->setEnabled(true);
    }
    else {
        deletePatchSectionAction->setText(tr("Delete section"));
        deletePatchSectionAction->setEnabled(false);
    }
    openEnclosingFolderAction->setEnabled(!filename.isEmpty());
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
    newAct = new QAction(icon("settings_input_composite"), tr("&New..."), this);
    newAct->setShortcut(QKeySequence(tr("Ctrl+Shift+Alt+N")));
    newAct->setStatusTip(tr("Create a new patch from scratch"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newPatch);
    fileMenu->addAction(newAct);
    toolbar->addAction(newAct);

    // Open
    openAct = new QAction(icon("open_in_browser"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing patch"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    toolbar->addAction(openAct);

    // Save
    saveAct = new QAction(icon("save"), tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save patch to file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    toolbar->addAction(saveAct);

    // Save as...
    saveAsAct = new QAction(tr("Save &as..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save patch to a different file"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    // Export selection
    exportSelectionAct = new QAction(tr("E&xport selection as patch..."), this);
    exportSelectionAct->setStatusTip(tr("Save the currently selected circuits into a new patch file"));
    connect(exportSelectionAct, &QAction::triggered, this, &MainWindow::exportSelection);
    fileMenu->addAction(exportSelectionAct);

    // Open enclosing folder
#if (defined Q_OS_MACOS || defined Q_OS_WIN)
#ifdef Q_OS_MACOS
    QString title = tr("Reveal in finder");
#else
    QString title = tr("Re&veal in explorer");
#endif
    openEnclosingFolderAction = new QAction(title, this);
    openEnclosingFolderAction->setStatusTip(tr("Open the folder where the current patch is located."));
    connect(openEnclosingFolderAction, &QAction::triggered, this, &MainWindow::openEnclosingFolder);
    fileMenu->addAction(openEnclosingFolderAction);
#endif

    // Recent files
    createRecentFileActions();

    // Integrate
    QAction *integrateAct = new QAction(icon("extension"), tr("&Integrate other patch as new section"), this);
    integrateAct->setShortcut(QKeySequence(tr("Ctrl+I")));
    integrateAct->setStatusTip(tr("Load another patch, add that as a new section after the currently selected section "
                               "and try to move the controls, inputs and outputs of that patch to unused "
                               "jacks and controlls"));
    connect(integrateAct, &QAction::triggered, this, &MainWindow::integrate);
    fileMenu->addAction(integrateAct);

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
    toolbar->addSeparator();
    editMenu = menuBar()->addMenu(ZERO_WIDTH_SPACE + tr("&Edit"));

    // Undo
    undoAction = new QAction(icon("undo"), tr("&Undo"), this);
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo last edit action"));
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAction);

    // Redo
    redoAction = new QAction(icon("redo"), tr("&Redo"), this);
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setStatusTip(tr("Redo last edit action"));
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);
    editMenu->addAction(redoAction);

    editMenu->addSeparator();

    // Copy & Paste
    cutAction = new QAction(icon("cut"), tr("C&ut"), this);
    cutAction->setShortcuts(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Cut selection to clipboard"));
    connect(cutAction, &QAction::triggered, &patchView, &PatchView::cut);
    editMenu->addAction(cutAction);

    copyAction = new QAction(icon("copy"), tr("&Copy"), this);
    copyAction->setShortcuts(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Copy selected stuff to clipboard"));
    connect(copyAction, &QAction::triggered, &patchView, &PatchView::copy);
    editMenu->addAction(copyAction);

    pasteAction = new QAction(icon("paste"), tr("&Paste"), this);
    pasteAction->setShortcuts(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Paste contents from clipboard"));
    connect(pasteAction, &QAction::triggered, &patchView, &PatchView::paste);
    editMenu->addAction(pasteAction);


    // New circuit...
    newCircuitAction = new QAction(icon("open_in_new"), tr("&New circuit..."), this);
    newCircuitAction->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));
    connect(newCircuitAction, &QAction::triggered, &patchView, &PatchView::newCircuit);
    editMenu->addAction(newCircuitAction);
    toolbar->addAction(newCircuitAction);

    // New jacks assignment
    addJackAction = new QAction(icon("settings_input_composite"), tr("&Add jack..."), this);
    addJackAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    connect(addJackAction, &QAction::triggered, &patchView, &PatchView::addJack);
    editMenu->addAction(addJackAction);
    toolbar->addAction(addJackAction);

    // Edit current line / field
    editValueAction = new QAction(icon("edit"), tr("&Edit element under cursor..."), this);
    editValueAction->setShortcuts({
                                    QKeySequence(tr("Enter")),
                                    QKeySequence(tr("Return"))});
    editMenu->addAction(editValueAction);
    connect(editValueAction, &QAction::triggered, &patchView, &PatchView::editValue);

    // Edit comment of current circuit
    editCircuitCommentAction = new QAction(tr("Edit circuit comment..."), this);
    editCircuitCommentAction->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));
    editMenu->addAction(editCircuitCommentAction);
    connect(editCircuitCommentAction, &QAction::triggered, &patchView, &PatchView::editCircuitComment);

    editMenu->addSeparator();

    // Add section
    addPatchSectionAction = new QAction(tr("Add section..."), this);
    editMenu->addAction(addPatchSectionAction);
    connect(addPatchSectionAction, &QAction::triggered, &patchView, &PatchView::addSection);

    // Rename section
    renamePatchSectionAction = new QAction(tr("Rename section..."), this);
    editMenu->addAction(renamePatchSectionAction);
    connect(renamePatchSectionAction, &QAction::triggered, &patchView, &PatchView::renameCurrentSection);

    // Delete section
    deletePatchSectionAction = new QAction(tr("Delete section"), this);
    editMenu->addAction(deletePatchSectionAction);
    connect(deletePatchSectionAction, &QAction::triggered, &patchView, &PatchView::deleteCurrentSection);

}

void MainWindow::createViewMenu()
{
    viewMenu = menuBar()->addMenu(tr("&View"));

    zoomResetAction = new QAction(icon("zoom_in"), tr("Normal font size"), this);
    zoomResetAction->setShortcut(QKeySequence(tr("Ctrl+0")));
    viewMenu->addAction(zoomResetAction);
    connect(zoomResetAction, &QAction::triggered, &patchView, [this] () { patchView.zoom(0); });

    zoomInAction = new QAction(icon("zoom_in"), tr("Increase font size"), this);
    zoomInAction->setShortcuts(QKeySequence::ZoomIn);
    viewMenu->addAction(zoomInAction);
    connect(zoomInAction, &QAction::triggered, &patchView, [this] () { patchView.zoom(1); });

    zoomOutAction = new QAction(icon("zoom_out"), tr("Outcrease font size"), this);
    zoomOutAction->setShortcuts(QKeySequence::ZoomOut);
    viewMenu->addAction(zoomOutAction);
    connect(zoomOutAction, &QAction::triggered, &patchView, [this] () { patchView.zoom(-1); });
}

void MainWindow::createRackMenu()
{
    QMenu *rackMenu = menuBar()->addMenu(tr("&Rack"));

    // Add controller
    addControllerAction = new QAction(icon("keyboard"), tr("&Add controller..."), this);
    addControllerAction->setShortcut(QKeySequence(tr("Ctrl+Alt+N")));
    connect(addControllerAction, &QAction::triggered, &rackView, &RackView::addController);
    rackMenu->addAction(addControllerAction);
    toolbar->addSeparator();
    toolbar->addAction(addControllerAction);
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
    patchHasChanged();
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
        patchHasChanged();
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
        patchHasChanged();
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
        rackView.setPatch(patch);
        patchView.setPatch(patch);
        patchHasChanged();
    }
}


void MainWindow::redo()
{
    if (undoHistory.redoPossible()) {
        if (patch)
            delete patch;
        patch = undoHistory.redo();
        rackView.setPatch(patch);
        patchView.setPatch(patch);
        patchHasChanged();
    }
}


void MainWindow::splitterMoved()
{
    QSettings settings;
    settings.setValue("mainwindow/splitposition", splitter->saveState());
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
