#include "circuitchoosedialog.h"
#include "circuitcollection.h"
#include "droidfirmware.h"
#include "tuning.h"

#include <QGridLayout>
#include <QAction>
#include <QLabel>
#include <QKeyEvent>
#include <QSettings>

#define TAB_INDEX_SEARCH 0
#define TAB_INDEX_FIRST_CATEGORY 1

CircuitChooseDialog::CircuitChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    QSettings settings;

    resize(CICH_DIALOG_WIDTH, CICH_DIALOG_HEIGHT);


    // The tab widget contains one tab for every category of circuits
    tabWidget = new QTabWidget(this);
    searchResults = new CircuitCollection(this);
    tabWidget->addTab(searchResults, tr("SEARCH"));
    connect(searchResults, &CircuitCollection::selectCircuit, this, &CircuitChooseDialog::accept);
    addCategoryTab("modulation", tr("Modulation"));
    addCategoryTab("sequencing", tr("Sequencing"));
    addCategoryTab("cv", tr("&CV Processing"));
    addCategoryTab("ui", tr("Knobs && Buttons"));
    addCategoryTab("clock", tr("Clocks && Triggers"));
    addCategoryTab("logic", tr("Math && Logic"));
    addCategoryTab("pitch", tr("Pitch"));
    addCategoryTab("midi", tr("MIDI"));
    addCategoryTab("other", tr("Other"));
    tabWidget->setTabVisible(TAB_INDEX_SEARCH, false);
    tabWidget->setCurrentIndex(TAB_INDEX_FIRST_CATEGORY);

    // The "Start jacks" choice determines with which jack assignments
    // should the new circuit start its life.
    startJacksBox = new QComboBox(this);
    // The order is imporant here. It must match the numbers
    // of jackselection_t
    startJacksBox->addItem(tr("Start with all available jacks"));
    startJacksBox->addItem(tr("Start with typical example"));
    startJacksBox->addItem(tr("Start with essential jacks"));
    startJacksBox->addItem(tr("Don't start with any jacks"));
    startJacksBox->setCurrentIndex(settings.value("circuitchooser/startjacks", 1).toInt());
    connect(startJacksBox, &QComboBox::currentIndexChanged, this, &CircuitChooseDialog::saveSettings);

    // Search
    QLabel *label = new QLabel(tr("Search:"), this);
    lineEditSearch = new QLineEdit(this);
    connect(lineEditSearch, &QLineEdit::textChanged, this, &CircuitChooseDialog::searchChanged);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Construct layout
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(tabWidget, 0, 0, 1, -1);
    mainLayout->addWidget(startJacksBox, 1, 0);
    mainLayout->addWidget(label, 1, 1);
    mainLayout->addWidget(lineEditSearch, 1, 2);
    mainLayout->addWidget(buttonBox, 1, 3);
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

jackselection_t CircuitChooseDialog::getJackSelection() const
{
    return (jackselection_t)startJacksBox->currentIndex();
}

void CircuitChooseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
        lineEditSearch->insert(event->text());
    else if (event->key() == Qt::Key_Backspace)
        lineEditSearch->backspace();
    else
        QDialog::keyPressEvent(event);

}

void CircuitChooseDialog::accept()
{
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
    if (tabWidget->currentIndex() == TAB_INDEX_SEARCH && lineEditSearch->text().isEmpty())
        nextCategory();
}


void CircuitChooseDialog::previousCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count());
    if (tabWidget->currentIndex() == TAB_INDEX_SEARCH && lineEditSearch->text().isEmpty())
        previousCategory();
}

void CircuitChooseDialog::searchChanged(QString text)
{
    if (text.isEmpty()) {
        tabWidget->setTabVisible(TAB_INDEX_SEARCH, false);
        tabWidget->setCurrentIndex(TAB_INDEX_FIRST_CATEGORY); // not search
    }
    else {
        tabWidget->setCurrentIndex(TAB_INDEX_SEARCH);
        searchResults->updateSearch(text);
        tabWidget->setTabVisible(TAB_INDEX_SEARCH, true);
    }
}

void CircuitChooseDialog::saveSettings()
{
    QSettings settings;
    settings.setValue("circuitchooser/startjacks", startJacksBox->currentIndex());
}
