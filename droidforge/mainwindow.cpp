#include "mainwindow.h"
#include "patch.h"
#include "ui_mainwindow.h"
#include "rackview.h"
#include "modulebuilder.h"
#include "patchparser.h"
#include "patchview.h"

#include <QSplitter>
#include <QTextEdit>


MainWindow::MainWindow(Rack *rack, Patch *patch)
    : QMainWindow()
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);

    RackView *rackview = new RackView(rack);
    PatchView *patchview = new PatchView(patch);
    splitter->addWidget(rackview);
    splitter->addWidget(patchview);
    createActions();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
        openAct->setShortcuts(QKeySequence::Open);
        openAct->setStatusTip(tr("Open an existing file"));
        connect(openAct, &QAction::triggered, this, &MainWindow::open);
        fileMenu->addAction(openAct);
}

void MainWindow::open()
{
    // if (maybeSave()) {
    //     QString fileName = QFileDialog::getOpenFileName(this);
    //     if (!fileName.isEmpty())
    //         loadFile(fileName);
    // }
}

bool MainWindow::maybeSave()
{
    return true;
}

void MainWindow::loadFile(const QString &)
{
}
