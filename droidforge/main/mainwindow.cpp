#include "mainwindow.h"
#include "clipboardindicator.h"
#include "cablestatusindicator.h"
#include "patchproblemindicator.h"
#include "parseexception.h"
#include "patch.h"
#include "rackview.h"
#include "modulebuilder.h"
#include "patchparser.h"
#include "patchpropertiesdialog.h"
#include "tuning.h"
#include "os.h"
#include "updatehub.h"
#include "patchsectionview.h"

#include <QTextEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QSettings>
#include <QtGlobal>
#include <QProcess>

MainWindow *the_forge;

MainWindow::MainWindow(const QString &initialFilename)
    : QMainWindow()
    , patch(new VersionedPatch())
    , patchSectionView(patch)
    , initialFilename(initialFilename)
{
    the_forge = this;

    setWindowTitle(APPLICATION_NAME);

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

    resize(800, 600);
    QSettings settings;
    if (settings.contains("mainwindow/splitposition"))
        rackSplitter->restoreState(settings.value("mainwindow/splitposition").toByteArray());
    connect(rackSplitter, &QSplitter::splitterMoved, this, &MainWindow::splitterMoved);

    createMenus();
    createToolbar();
    createStatusBar();
    connectActions();

    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [&] () {loadFile(initialFilename, FILE_MODE_LOAD);});

    // Events that we create
    connect(this, &MainWindow::patchChanged, the_hub, &UpdateHub::changePatch);
    connect(this, &MainWindow::patchModified, the_hub, &UpdateHub::modifyPatch);

    // Events that we are interested in
}


void MainWindow::createStatusBar()
{
    statusbar = new QStatusBar(this);
    setStatusBar(statusbar);

    statusbar->addPermanentWidget(new CableStatusIndicator);
    statusbar->addPermanentWidget(new PatchProblemIndicator);
    statusbar->addPermanentWidget(new ClipboardIndicator);
}

MainWindow::~MainWindow()
{
    delete patch;
}

void MainWindow::integratePatch(const QString &)
{
    /* Patch otherpatch;
    parser.parse(aFilePath, &otherpatch);
    Patch *newPatch = patchView.integratePatch(&otherpatch); // TODO: Move actual action to PV
    if (newPatch) {
        registerEdit(tr("integrating other patch '%1'").arg(otherpatch.getTitle()));
        patchHasChanged();
    }*/
}

void MainWindow::registerEdit(QString name)
{
    qDebug() << "ACHTUNG! DEPRIZIER" << name;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        // patchView.abortAllActions(); // TODO
        // rackView.abortAllAction();
    }
    else  if (!patchSectionView.handleKeyPress(event)) {
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
        if (how == FILE_MODE_LOAD)
            loadPatch(filename);
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

void MainWindow::createMenus()
{
    createFileMenu();
    createRackMenu();
    createEditMenu();
    createViewMenu();

    // TODO: looks strange to me
    ADD_ACTION(ACTION_NEXT_SECTION, this); // make shortcuts works
    ADD_ACTION(ACTION_PREVIOUS_SECTION, this); // make shortcuts works
}


void MainWindow::patchHasChanged()
{
    qDebug() << Q_FUNC_INFO;
    Q_ASSERT(false);
}

void MainWindow::hiliteRegisters(const RegisterList &registers)
{
    rackView.hiliteRegisters(registers);
}

void MainWindow::clickOnRegister(AtomRegister ar)
{
    patchSectionView.clickOnRegister(ar);
}

void MainWindow::updateWindowTitle()
{
    QString title;
    if (filePath.isEmpty())
        title = tr("(untitled patch)") + " - " + tr("DROID Forge");
    else
        title = filePath + " - " + tr("DROID Forge");
    if (patch->isModified())
        title += " (" + tr("modified") + ")";
    setWindowTitle(title);
}



void MainWindow::repaintPatchView()
{
    patchSectionView.updateCircuits(); // TODO: ?Signale
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
    QMenu *menu = menuBar()->addMenu(tr("&File"));

    ADD_ACTION(ACTION_NEW, menu);
    ADD_ACTION(ACTION_OPEN, menu);
    ADD_ACTION(ACTION_SAVE, menu);
    ADD_ACTION(ACTION_SAVE_AS, menu);
    ADD_ACTION(ACTION_EXPORT_SELECTION, menu);

    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    ADD_ACTION(ACTION_OPEN_ENCLOSING_FOLDER, menu);
    #endif

    createRecentFileActions(menu);

    ADD_ACTION(ACTION_INTEGRATE_PATCH, menu);
    ADD_ACTION(ACTION_JUMP_TO_NEXT_PROBLEM, menu);
    ADD_ACTION(ACTION_QUIT, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_PATCH_PROPERTIES, menu);
}

void MainWindow::createRecentFileActions(QMenu *fileMenu)
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
    QMenu *menu = menuBar()->addMenu(ZERO_WIDTH_SPACE + tr("&Edit"));

    ADD_ACTION(ACTION_UNDO, menu);
    ADD_ACTION(ACTION_REDO, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_CUT, menu);
    ADD_ACTION(ACTION_COPY, menu);
    ADD_ACTION(ACTION_PASTE, menu);
    ADD_ACTION(ACTION_PASTE_SMART, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_NEW_CIRCUIT, menu);
    ADD_ACTION(ACTION_ADD_JACK, menu);
    ADD_ACTION(ACTION_EDIT_VALUE, menu);
    ADD_ACTION(ACTION_FOLLOW_INTERNAL_CABLE, menu);
    ADD_ACTION(ACTION_RENAME_INTERNAL_CABLE, menu);
    ADD_ACTION(ACTION_START_PATCHING, menu);
    ADD_ACTION(ACTION_FINISH_PATCHING, menu);
    ADD_ACTION(ACTION_ABORT_PATCHING, menu);
    ADD_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, menu);

    menu->addSeparator();

    ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_RENAME_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);
    ADD_ACTION(ACTION_MOVE_INTO_SECTION, menu);
}

void MainWindow::createViewMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&View"));
    ADD_ACTION(ACTION_RESET_ZOOM, menu);
    ADD_ACTION(ACTION_ZOOM_IN, menu);
    ADD_ACTION(ACTION_ZOOM_OUT, menu);
}

void MainWindow::createRackMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&Rack"));
    ADD_ACTION(ACTION_ADD_CONTROLLER, menu);
}

void MainWindow::createToolbar()
{
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    ADD_ACTION(ACTION_NEW, toolbar);
    ADD_ACTION(ACTION_OPEN, toolbar);
    ADD_ACTION(ACTION_SAVE, toolbar);
    toolbar->addSeparator();
    ADD_ACTION(ACTION_NEW_CIRCUIT, toolbar);
    ADD_ACTION(ACTION_ADD_JACK, toolbar);
    ADD_ACTION(ACTION_ADD_CONTROLLER, toolbar);
}

void MainWindow::connectActions()
{
    CONNECT_ACTION(ACTION_NEW, &MainWindow::newPatch);
    CONNECT_ACTION(ACTION_OPEN, &MainWindow::open);
    CONNECT_ACTION(ACTION_SAVE, &MainWindow::save);
    CONNECT_ACTION(ACTION_SAVE_AS, &MainWindow::saveAs);
    CONNECT_ACTION(ACTION_EXPORT_SELECTION, &MainWindow::exportSelection);
    CONNECT_ACTION(ACTION_INTEGRATE_PATCH, &MainWindow::integrate);
    CONNECT_ACTION(ACTION_JUMP_TO_NEXT_PROBLEM, &MainWindow::jumpToNextProblem);
    CONNECT_ACTION(ACTION_QUIT, &MainWindow::close);
    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    CONNECT_ACTION(ACTION_OPEN_ENCLOSING_FOLDER, &MainWindow::openEnclosingFolder);
    #endif

    CONNECT_ACTION(ACTION_UNDO, &MainWindow::undo);
    CONNECT_ACTION(ACTION_REDO, &MainWindow::redo);

    CONNECT_ACTION(ACTION_PATCH_PROPERTIES, &MainWindow::editProperties);

}

void MainWindow::loadPatch(const QString &aFilePath)
{
    delete patch;
    Patch newPatch;
    parser.parse(aFilePath, &newPatch);
    patch = new VersionedPatch(&newPatch);
    filePath = aFilePath;
    emit patchChanged(patch);
}

void MainWindow::newPatch()
{
    if (!checkModified())
        return;

    delete patch;
    filePath = "";

    Patch newPatch;
    newPatch.addSection(new PatchSection(SECTION_DEFAULT_NAME));
    patch = new VersionedPatch(&newPatch);
    emit patchChanged(patch);
}

void MainWindow::open()
{
    if (!checkModified())
        return;

    QString filePath = QFileDialog::getOpenFileName(this);
    if (!filePath.isEmpty())
        loadFile(filePath, FILE_MODE_LOAD);
}

void MainWindow::integrate()
{
    QString filePath = QFileDialog::getOpenFileName(this);
    if (!filePath.isEmpty())
        loadFile(filePath, FILE_MODE_INTEGRATE);
}

void MainWindow::save()
{
    if (filePath.isEmpty())
        saveAs();
    else {
        patch->saveToFile(filePath);
        emit patchModified(); // mod flag
    }
}

void MainWindow::saveAs()
{
    QString newFilePath = QFileDialog::getSaveFileName(
                this,
                tr("Save patch to new file"),
                filePath,
                tr("DROID patch files (*.ini)"));
    if (!newFilePath.isEmpty()) {
        patch->saveToFile(newFilePath);
        filePath = newFilePath;
        emit patchModified(); // mod flag
        addToRecentFiles(newFilePath);
    }
}

void MainWindow::editProperties()
{
    if (PatchPropertiesDialog::editPatchProperties(patch))
        emit patchModified();
}

void MainWindow::exportSelection()
{
    QString filePath = QFileDialog::getSaveFileName(
                this,
                tr("Export selection into new patch"),
                "",
                tr("DROID patch files (*.ini)"));
    if (!filePath.isEmpty()) {
        Patch *patch = patchSectionView.getSelectionAsPatch();
        VersionedPatch vp(patch);
        vp.saveToFile(filePath);
        delete patch;
        addToRecentFiles(filePath);
    }
}

void MainWindow::openEnclosingFolder()
{
    QFileInfo fileinfo(filePath);
    openDirInFinder(fileinfo.absoluteFilePath());
}

void MainWindow::undo()
{
    if (patch->undoPossible()) {
        patch->undo();
        emit patchModified();
    }
}

void MainWindow::redo()
{
    if (patch->redoPossible()) {
        patch->redo();
        emit patchModified();
    }
}

void MainWindow::jumpToNextProblem()
{
    // const PatchProblem *problem = patch->problem(currentProblem++);
    // if (currentProblem >= patch->numProblems())
    //     currentProblem = 0;
    // TODO
    // patchView.jumpTo(problem->getSection(), problem->getCursorPosition());
}


void MainWindow::splitterMoved()
{
    QSettings settings;
    settings.setValue("mainwindow/splitposition", rackSplitter->saveState());
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
    if (patch->isModified()) {
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
    qDebug() << Q_FUNC_INFO << what << "loswerden!";
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
