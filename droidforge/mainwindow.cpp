#include "mainwindow.h"
#include "parseexception.h"
#include "patch.h"
#include "ui_mainwindow.h"
#include "rackview.h"
#include "modulebuilder.h"
#include "patchparser.h"
#include "patchview.h"

#include <QSplitter>
#include <QTextEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>

MainWindow *the_forge;
DroidFirmware *the_firmware;

MainWindow::MainWindow(const QString &initialFilename)
    : QMainWindow()
    , ui(new Ui::MainWindow)
    , initialFilename(initialFilename)
    , patch(0)
{
    resize(800,1000);
    move(1200, 0);

    the_forge = this;
    the_firmware = &firmware;

    ui->setupUi(this);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);

    splitter->addWidget(&rackview);
    splitter->addWidget(&patchview);

    createActions();

    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [&] () {slotLoadPatch(initialFilename);});
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
    undoHistory.clear();
    undoHistory.snapshot("Load from file", patch);
}


void MainWindow::registerEdit(QString name)
{
    undoHistory.snapshot(name, patch);
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
    if (!patchview.handleKeyPress(event->key())) {
        event->ignore();
        // QWidget::keyPressEvent(event);
    }
}

void MainWindow::slotLoadPatch(const QString &filename)
{
    try {
        loadPatch(filename);
    }
    catch (ParseException &e) {
        QMessageBox box;
        box.setText(MainWindow::tr("Cannot load ") + filename);
        box.setInformativeText(e.toString());
        box.setStandardButtons(QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Critical);
        box.setBaseSize(QSize(600, 220));
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
    nextSectionAct->setStatusTip(tr("Switch to the next patch section"));
    addAction(nextSectionAct);
    connect(nextSectionAct, &QAction::triggered, &patchview, &PatchView::nextSection);

    QAction *prevSectionAct = new QAction(tr("Previous section"));
    prevSectionAct->setShortcut(QKeySequence(tr("Ctrl+Left")));
    prevSectionAct->setStatusTip(tr("Switch to the previous patch section"));
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
    return;
}


void MainWindow::createFileMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    // Open
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);

    // Save
    const QIcon saveIcon = QIcon::fromTheme("document-open", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save patch to file"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);

    // Patch properties
    QAction *patchPropertiesAct = new QAction(saveIcon, tr("&Patch properties..."), this);
    patchPropertiesAct->setShortcut(QKeySequence(tr("Ctrl+.")));
    connect(patchPropertiesAct, &QAction::triggered, &patchview, &PatchView::editProperties);
    fileMenu->addAction(patchPropertiesAct);

}


void MainWindow::createEditMenu()
{
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    // Undo
    const QIcon undoIcon = QIcon::fromTheme("undo", QIcon(":/images/undo.png"));
    undoAction = new QAction(undoIcon, tr("&Undo"), this);
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo last edit action"));
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAction);

    // Redo
    const QIcon redoIcon = QIcon::fromTheme("redo", QIcon(":/images/redo.png"));
    redoAction = new QAction(redoIcon, tr("&Redo"), this);
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setStatusTip(tr("Redo last edit action"));
    connect(redoAction, &QAction::triggered, this, &MainWindow::redo);
    editMenu->addAction(redoAction);

    // New circuit...
    const QIcon newIcon = QIcon::fromTheme("new", QIcon(":/images/new.png"));
    QAction *newCircuitAction = new QAction(newIcon, tr("&New circuit..."), this);
    newCircuitAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    connect(newCircuitAction, &QAction::triggered, &patchview, &PatchView::newCircuit);
    editMenu->addAction(newCircuitAction);

}


void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::save()
{
    patch->saveToFile(filename + ".new");

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


bool MainWindow::maybeSave()
{
    return true;
}


void MainWindow::loadFile(const QString &)
{
}
