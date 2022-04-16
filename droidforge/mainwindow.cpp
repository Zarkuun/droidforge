#include "mainwindow.h"
#include "droidforge.h"
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
}


bool MainWindow::undoPossible()
{
    return undoHistory.size() > 1;
}


QString MainWindow::nextUndoTitle() const
{
    return undoHistory.nextTitle();
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
        qDebug() << "Unhandled Main Key press: " << event;
    }
}


void MainWindow::createActions()
{
    createFileMenu();
    createEditMenu();
    updateActions();
}


void MainWindow::updateActions()
{
    if (the_forge->undoPossible()) {
        undoAction->setText(tr("&Undo ") + the_forge->nextUndoTitle());
        undoAction->setEnabled(true);
    }
    else {
        undoAction->setText(tr("&Undo"));
        undoAction->setEnabled(false);
    }
}


void MainWindow::createFileMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
}


void MainWindow::createEditMenu()
{
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    const QIcon undoIcon = QIcon::fromTheme("undo", QIcon(":/images/undo.png"));
    undoAction = new QAction(undoIcon, tr("&Undo"), this);
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setStatusTip(tr("Undo last edit action"));
    connect(undoAction, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAction);
}


void MainWindow::open()
{
    qDebug("OPEN SOLL PASSIEREN");
    // if (maybeSave()) {
    //     QString fileName = QFileDialog::getOpenFileName(this);
    //     if (!fileName.isEmpty())
    //         loadFile(fileName);
    // }
}

void MainWindow::undo()
{
    if (undoPossible()) {
        if (patch)
            delete patch;
        patch = undoHistory.undo();
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
