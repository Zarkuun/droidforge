#include "circuitchoosedialog.h"
#include "circuitcollection.h"
#include "droidfirmware.h"

#include <QGridLayout>
#include <QAction>

CircuitChooseDialog::CircuitChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    resize(800, 500);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                     this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


    tabWidget = new QTabWidget(this);
    addCategoryTab("modulation", tr("Modulation"));
    addCategoryTab("sequencing", tr("Sequencing"));
    addCategoryTab("cv", tr("&CV Processing"));
    addCategoryTab("ui", tr("Knobs && Buttons"));
    addCategoryTab("clock", tr("Clocks && Triggers"));
    addCategoryTab("logic", tr("Math && Logic"));
    addCategoryTab("pitch", tr("Pitch"));
    addCategoryTab("midi", tr("MIDI"));
    addCategoryTab("other", tr("Other"));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Add new circuit"));

    QAction *nextCategoryAct = new QAction(tr("Next category"));
    nextCategoryAct->setShortcut(QKeySequence(tr("Ctrl+Right")));
    addAction(nextCategoryAct);
    connect(nextCategoryAct, &QAction::triggered, this, &CircuitChooseDialog::nextCategory);

    QAction *previousCategoryAct = new QAction(tr("Previous category"));
    previousCategoryAct->setShortcut(QKeySequence(tr("Ctrl+Left")));
    addAction(previousCategoryAct);
    connect(previousCategoryAct, &QAction::triggered, this, &CircuitChooseDialog::previousCategory);
}


CircuitChooseDialog::~CircuitChooseDialog()
{

}

void CircuitChooseDialog::addCategoryTab(QString /* category */, QString title)
{
    CircuitCollection *cc = new CircuitCollection(this);
    tabWidget->addTab(cc, title);

    // QStringList circuits = the_firmware->circuitsOfCategory("modulation");
    // for (qsizetype i=0; i<circuits.size(); i++) {
    //     qDebug() << "HAB" << circuits[i];
    // }
}

void CircuitChooseDialog::nextCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() + 1) % tabWidget->count());
}


void CircuitChooseDialog::previousCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count());
}
