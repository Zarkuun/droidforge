#include "circuitchoosedialog.h"
#include "circuitcollection.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QGridLayout>
#include <QAction>

CircuitChooseDialog::CircuitChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    resize(CICH_DIALOG_WIDTH, CICH_DIALOG_HEIGHT);
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
    QList<QKeySequence> s1;
    s1.append(QKeySequence(tr("Right")));
    s1.append(QKeySequence(tr("Ctrl+Right")));
    nextCategoryAct->setShortcuts(s1);
    addAction(nextCategoryAct);
    connect(nextCategoryAct, &QAction::triggered, this, &CircuitChooseDialog::nextCategory);

    QAction *previousCategoryAct = new QAction(tr("Previous category"));
    QList<QKeySequence> s2;
    s2.append(QKeySequence(tr("Left")));
    s2.append(QKeySequence(tr("Ctrl+Left")));
    previousCategoryAct->setShortcuts(s2);
    addAction(previousCategoryAct);
    connect(previousCategoryAct, &QAction::triggered, this, &CircuitChooseDialog::previousCategory);
}


CircuitChooseDialog::~CircuitChooseDialog()
{

}

QString CircuitChooseDialog::getSelectedCircuit() const
{
    CircuitCollection *collection = (CircuitCollection *)tabWidget->currentWidget();
    return collection->selectedCircuitName();
}

void CircuitChooseDialog::accept()
{
    qDebug() << Q_FUNC_INFO;
    QDialog::accept();
}


void CircuitChooseDialog::addCategoryTab(QString category, QString title)
{
    CircuitCollection *cc = new CircuitCollection(category, this);
    tabWidget->addTab(cc, title);
    connect(cc, &CircuitCollection::selectCircuit, this, &CircuitChooseDialog::accept);
}

void CircuitChooseDialog::nextCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() + 1) % tabWidget->count());
}


void CircuitChooseDialog::previousCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count());
}
