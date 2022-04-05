#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rackview.h"

#include <QFileDialog>
#include <QSplitter>
#include <QTextEdit>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);
    QTextEdit *textedit1 = new QTextEdit();
    RackView *rackview = new RackView();
    splitter->addWidget(rackview);
    splitter->addWidget(textedit1);
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
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::maybeSave()
{
    return true;
}

void MainWindow::loadFile(const QString &)
{
}
