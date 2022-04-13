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


MainWindow::MainWindow(Rack *rack, Patch *patch)
    : QMainWindow()
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);

    rackview = new RackView(rack);
    patchview = new PatchView(patch);
    splitter->addWidget(rackview);
    splitter->addWidget(patchview);
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
    if (!patchview->handleKeyPress(event->key())) {
        qDebug() << "Unhandled Main Key press: " << event;
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "Main Key release: " << event;

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
