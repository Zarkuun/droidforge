#include "mainwindow.h"
#include "parseexception.h"
#include "patch.h"
#include "ui_mainwindow.h"
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
    , ui(new Ui::MainWindow)
    , initialFilename(initialFilename)
    , patch(0)
{
    the_forge = this;
    the_firmware = &firmware;

    ui->setupUi(this);
    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);
    splitter->addWidget(&rackview);
    splitter->addWidget(&patchview);
    splitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);
    QSettings settings;
    if (settings.contains("mainwindow/splitposition"))
        splitter->restoreState(settings.value("mainwindow/splitposition").toByteArray());
    connect(splitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    createActions();

    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [&] () {loadFile(initialFilename);});


}


MainWindow::~MainWindow()
{
    if (patch)
        delete patch;
    delete ui;
}


void MainWindow::loadPatch(QString afilename)
{
    Patch newpatch;
    parser.parse(afilename, &newpatch);
    setPatch(newpatch.clone());

    filename = afilename;
    undoHistory.reset(&newpatch);
    updateActions();
}


void MainWindow::registerEdit(QString name)
{
    undoHistory.snapshot(patch, name);
    updateActions();
}


void MainWindow::setPatch(Patch *newpatch)
{
    if (patch)
        delete patch;
    patch = newpatch;
    rackview.setPatch(patch);
    patchview.setPatch(patch);
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << event;
    if (!patchview.handleKeyPress(event->key())) {
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


void MainWindow::loadFile(const QString &filename)
{
    if (!checkModified())
        return;

    try {
        addToRecentFiles(filename);
        loadPatch(filename);

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
    createEditMenu();
    updateActions();

    QAction *nextSectionAct = new QAction(tr("Next section"));
    nextSectionAct->setShortcut(QKeySequence(tr("Ctrl+Right")));
    nextSectionAct->setStatusTip(tr("Switch to the next section"));
    addAction(nextSectionAct);
    connect(nextSectionAct, &QAction::triggered, &patchview, &PatchView::nextSection);

    QAction *prevSectionAct = new QAction(tr("Previous section"));
    prevSectionAct->setShortcut(QKeySequence(tr("Ctrl+Left")));
    prevSectionAct->setStatusTip(tr("Switch to the previous section"));
    addAction(prevSectionAct);
    connect(prevSectionAct, &QAction::triggered, &patchview, &PatchView::previousSection);
}


void MainWindow::updateActions()
{
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

    const PatchSectionView *psv = patchview.patchSectionView();
    bool empty = !psv || psv->isEmpty();
    addJackAction->setEnabled(!empty);
    editValueAction->setEnabled(!empty);
    editCircuitCommentAction->setEnabled(!empty);
    if (patchview.numSections() > 1) {
        deletePatchSectionAction->setText(tr("Delete section") + " '" + psv->getTitle() + "'");
        deletePatchSectionAction->setEnabled(true);
    }
    else {
        deletePatchSectionAction->setText(tr("Delete section"));
        deletePatchSectionAction->setEnabled(false);
    }

    openEnclosingFolderAction->setEnabled(!filename.isEmpty());

    QString title;
    if (filename.isEmpty())
        title = tr("(untitled patch)") + " - " + tr("DROID Forge");
    else
        title = filename + " - " + tr("DROID Forge");
    if (undoHistory.isModified())
        title += " (" + tr("modified") + ")";
    setWindowTitle(title);
}


void MainWindow::createFileMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    // New
    QAction *newAct = new QAction(icon("settings_input_composite"), tr("&New..."), this);
    newAct->setShortcut(QKeySequence(tr("Ctrl+Shift+Alt+N")));
    newAct->setStatusTip(tr("Create a new patch from scratch"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newPatch);
    fileMenu->addAction(newAct);
    toolbar->addAction(newAct);

    // Open
    QAction *openAct = new QAction(icon("open_in_browser"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing patch"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    toolbar->addAction(openAct);

    // Save
    QAction *saveAct = new QAction(icon("save"), tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save patch to file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    toolbar->addAction(saveAct);

    // Save as...
    QAction *saveAsAct = new QAction(tr("Save &as..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save patch to a different file"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

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
    connect(patchPropertiesAct, &QAction::triggered, &patchview, &PatchView::editProperties);
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
        connect(action, &QAction::triggered, this, [this, path]() { this->loadFile(path); });
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
    QMenu *editMenu = menuBar()->addMenu(ZERO_WIDTH_SPACE + tr("&Edit"));

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

    // New circuit...
    newCircuitAction = new QAction(icon("open_in_new"), tr("&New circuit..."), this);
    newCircuitAction->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));
    connect(newCircuitAction, &QAction::triggered, &patchview, &PatchView::newCircuit);
    editMenu->addAction(newCircuitAction);
    toolbar->addAction(newCircuitAction);

    // New jacks assignment
    addJackAction = new QAction(icon("settings_input_composite"), tr("&Add jack..."), this);
    addJackAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    connect(addJackAction, &QAction::triggered, &patchview, &PatchView::addJack);
    editMenu->addAction(addJackAction);
    toolbar->addAction(addJackAction);

    // Edit current line / field
    editValueAction = new QAction(icon("edit"), tr("&Edit element under cursor..."), this);
    editValueAction->setShortcuts({
                                    QKeySequence(tr("Enter")),
                                    QKeySequence(tr("Return"))});
    editMenu->addAction(editValueAction);
    connect(editValueAction, &QAction::triggered, &patchview, &PatchView::editValue);

    // Edit comment of current circuit
    editCircuitCommentAction = new QAction(tr("Edit circuit comment..."), this);
    editCircuitCommentAction->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));
    editMenu->addAction(editCircuitCommentAction);
    connect(editCircuitCommentAction, &QAction::triggered, &patchview, &PatchView::editCircuitComment);

    editMenu->addSeparator();

    // Add section
    addPatchSectionAction = new QAction(tr("Add section..."), this);
    editMenu->addAction(addPatchSectionAction);
    connect(addPatchSectionAction, &QAction::triggered, &patchview, &PatchView::addSection);

    // Rename section
    renamePatchSectionAction = new QAction(tr("Rename section..."), this);
    editMenu->addAction(renamePatchSectionAction);
    connect(renamePatchSectionAction, &QAction::triggered, &patchview, &PatchView::renameCurrentSection);

    // Delete section
    deletePatchSectionAction = new QAction(tr("Delete section"), this);
    editMenu->addAction(deletePatchSectionAction);
    connect(deletePatchSectionAction, &QAction::triggered, &patchview, &PatchView::deleteCurrentSection);

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
    updateActions();
}

void MainWindow::open()
{
    if (!checkModified())
        return;

    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::save()
{
    if (filename.isEmpty())
        saveAs();
    else {
        patch->saveToFile(filename);
        undoHistory.clearModified();
        updateActions();
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
        updateActions();
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
        rackview.setPatch(patch);
        patchview.setPatch(patch);
        updateActions();
    }
}


void MainWindow::redo()
{
    if (undoHistory.redoPossible()) {
        if (patch)
            delete patch;
        patch = undoHistory.redo();
        rackview.setPatch(patch);
        patchview.setPatch(patch);
        updateActions();
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
        patchview.releaseKeyboard();
        QMessageBox box(
                    QMessageBox::Warning,
                    tr("Your patch is modified!"),
                    tr("Do you want to save your changes before you proceed?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    this);
        int ret = box.exec();
        patchview.grabKeyboard();
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
    qDebug() << filename;
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
