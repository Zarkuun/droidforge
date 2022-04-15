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


MainWindow::MainWindow(Patch *patch)
    : QMainWindow()
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);

    rackview.setPatch(patch);
    patchview.setPatch(patch);
    splitter->addWidget(&rackview);
    splitter->addWidget(&patchview);
    splitter->grabKeyboard(); // Macht, dass bei main die Tasten ankommen
    createActions();
}


MainWindow::~MainWindow()
{
    delete ui;
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
    QAction *undoAct = new QAction(undoIcon, tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo last edit action"));
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);
    editMenu->addAction(undoAct);
}


void MainWindow::open()
{
    // if (maybeSave()) {
    //     QString fileName = QFileDialog::getOpenFileName(this);
    //     if (!fileName.isEmpty())
    //         loadFile(fileName);
    // }
}

void MainWindow::undo()
{
    if (the_forge->undoPossible()) {
        the_forge->undo();
        Patch *patch = the_forge->getPatch();
        rackview.setPatch(patch);
        patchview.setPatch(patch);
    }
}

bool MainWindow::maybeSave()
{
    return true;
}

void MainWindow::loadFile(const QString &)
{
}
