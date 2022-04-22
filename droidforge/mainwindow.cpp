#include "mainwindow.h"
#include "patch.h"
#include "ui_mainwindow.h"
#include "rackview.h"
#include "modulebuilder.h"
#include "patchparser.h"
#include "patchview.h"

#include <QSplitter>
#include <QTextEdit>
#include <QKeyEvent>

MainWindow *the_forge;
DroidFirmware *the_firmware;

MainWindow::MainWindow()
    : QMainWindow()
    , ui(new Ui::MainWindow)
    , patch(0)
{
    the_forge = this;
    the_firmware = &firmware;

    ui->setupUi(this);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);

    splitter->addWidget(&rackview);
    splitter->addWidget(&patchview);
    splitter->grabKeyboard(); // Macht, dass bei main die Tasten ankommen
    createActions();
}


MainWindow::~MainWindow()
{
    if (patch)
        delete patch;
    delete ui;
}


bool MainWindow::loadPatch(QString afilename)
{
    Patch newpatch;
    if (!parser.parse(afilename, &newpatch))
        return false;

    setPatch(newpatch.clone());

    filename = afilename;
    undoHistory.clear();
    undoHistory.snapshot("Load from file", patch);
    return true;
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
    qDebug() << "Key" << event;
    if (!patchview.handleKeyPress(event->key())) {
        qDebug("Unhandeld");
        QMainWindow::keyPressEvent(event);
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
}


void MainWindow::open()
{
    // if (maybeSave()) {
    //     QString fileName = QFileDialog::getOpenFileName(this);
    //     if (!fileName.isEmpty())
    //         loadFile(fileName);
    // }
}

void MainWindow::save()
{
    qDebug() << "saving" << filename;
    patch->saveToFile(filename);

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
