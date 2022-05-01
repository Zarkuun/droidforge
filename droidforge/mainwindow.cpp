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
#include <QSettings>

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
    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    this->setCentralWidget(splitter);

    splitter->addWidget(&rackview);
    splitter->addWidget(&patchview);

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

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


void MainWindow::closeEvent(QCloseEvent *)
{
    QSettings settings;
    settings.setValue("mainwindow/position", pos());
    settings.setValue("mainwindow/size", size());
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

    QAction *openAct = new QAction(icon("open_in_browser"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
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

    // Patch properties
    QAction *patchPropertiesAct = new QAction(icon("dns"), tr("&Patch properties..."), this);
    patchPropertiesAct->setShortcut(QKeySequence(tr("Ctrl+.")));
    connect(patchPropertiesAct, &QAction::triggered, &patchview, &PatchView::editProperties);
    fileMenu->addAction(patchPropertiesAct);
}


void MainWindow::createEditMenu()
{
    toolbar->addSeparator();
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

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
    QAction *newCircuitAction = new QAction(icon("open_in_new"), tr("&New circuit..."), this);
    newCircuitAction->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));
    connect(newCircuitAction, &QAction::triggered, &patchview, &PatchView::newCircuit);
    editMenu->addAction(newCircuitAction);
    toolbar->addAction(newCircuitAction);

    // New jacks assignment
    QAction *addJackAction = new QAction(icon("settings_input_composite"), tr("&Add jack..."), this);
    addJackAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    connect(addJackAction, &QAction::triggered, &patchview, &PatchView::addJack);
    editMenu->addAction(addJackAction);
    toolbar->addAction(addJackAction);
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

QIcon MainWindow::icon(QString what) const
{
    return QIcon(":/images/icons/white/" + what + ".png");
}
