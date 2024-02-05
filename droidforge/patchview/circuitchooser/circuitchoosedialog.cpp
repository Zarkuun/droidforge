#include "circuitchoosedialog.h"
#include "circuitcollection.h"
#include "os.h"
#include "droidfirmware.h"
#include "globals.h"
#include "tuning.h"
#include "usermanual.h"

#include <QGridLayout>
#include <QAction>
#include <QLabel>
#include <QKeyEvent>
#include <QSettings>

#define TAB_INDEX_SEARCH 0
#define TAB_INDEX_FIRST_CATEGORY 1

CircuitChooseDialog::CircuitChooseDialog(QWidget *parent)
    : Dialog("circuitchooser", parent)
{
    QSettings settings;

    setWindowTitle(tr("Add new circuit"));

    // The tab widget contains one tab for every category of circuits
    tabWidget = new QTabWidget(this);
    searchResults = new CircuitCollection(this);
    tabWidget->addTab(searchResults, tr("SEARCH"));
    connect(searchResults, &CircuitCollection::selectCircuit, this, &CircuitChooseDialog::accept);
    addCategoryTab("modulation", tr("Modulation"));
    addCategoryTab("sequencing", tr("Sequencing"));
    addCategoryTab("cv", tr("&CV Processing"));
    addCategoryTab("ui", tr("Controls"));
    addCategoryTab("clock", tr("Clocks / Triggers"));
    addCategoryTab("logic", tr("Math / Logic"));
    addCategoryTab("pitch", tr("Pitch"));
    addCategoryTab("midi", tr("MIDI"));
    addCategoryTab("other", tr("Other"));
    // addCategoryTab("deprecated", tr("Deprecated"));
    tabWidget->setTabVisible(TAB_INDEX_SEARCH, false);
    tabWidget->setCurrentIndex(TAB_INDEX_FIRST_CATEGORY);
    tabWidget->setFocusPolicy(Qt::NoFocus);

    // The "Start jacks" choice determines with which jack assignments
    // should the new circuit start its life.
    startJacksBox = new QComboBox(this);
    // The order is imporant here. It must match the numbers
    // of jackselection_t
    startJacksBox->addItem(tr("Start with all available parameters"));
    startJacksBox->addItem(tr("Start with typical example"));
    startJacksBox->addItem(tr("Start with essential parameters"));
    startJacksBox->addItem(tr("Don't start with any parameters"));
    startJacksBox->setCurrentIndex(settings.value("circuitchooser/startjacks", 1).toInt());
    connect(startJacksBox, &QComboBox::currentIndexChanged, this, &CircuitChooseDialog::saveSettings);

    // Search
    QLabel *label = new QLabel(tr("Search:"), this);
    lineEditSearch = new KeyCaptureLineEdit(this);
    connect(lineEditSearch, &QLineEdit::textChanged, this, &CircuitChooseDialog::searchChanged);
    connect(lineEditSearch, &KeyCaptureLineEdit::keyPressed, this, &CircuitChooseDialog::keyPressed);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *manualButton = new QPushButton(tr("Manual"));
    connect(manualButton, &QPushButton::clicked, this, &CircuitChooseDialog::showManual);
    buttonBox->addButton(manualButton, QDialogButtonBox::ActionRole);
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

    setFocusPolicy(Qt::NoFocus);
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
    if (event->key() == Qt::Key_M && (event->modifiers() & Qt::ControlModifier)) {
        showManual();
        return;
    }
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
        lineEditSearch->insert(event->text());
    else if (event->key() == Qt::Key_Backspace)
        lineEditSearch->backspace();
    else
        QDialog::keyPressEvent(event);
}
void CircuitChooseDialog::showEvent(QShowEvent *)
{
    lineEditSearch->selectAll();
    lineEditSearch->setFocus();
}
void CircuitChooseDialog::accept()
{
    QDialog::accept();
}
QString CircuitChooseDialog::chooseCircuit(jackselection_t &jsel)
{
    return chooseCircuit(jsel, "");
}
QString CircuitChooseDialog::chooseCircuit(QString oldCircuit)
{
    jackselection_t jsel;
    return chooseCircuit(jsel, oldCircuit);
}
QString CircuitChooseDialog::chooseCircuit(jackselection_t &jsel, QString oldCircuit)
{
    static CircuitChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new CircuitChooseDialog();

    if (!oldCircuit.isEmpty())
        dialog->setCurrentCircuit(oldCircuit);
    if (dialog->exec() == QDialog::Accepted) {
        jsel = dialog->getJackSelection();
        return dialog->getSelectedCircuit();
    }
    else
        return "";
}
void CircuitChooseDialog::addCategoryTab(QString category, QString title)
{
    CircuitCollection *cc = new CircuitCollection(category, this);
    tabWidget->addTab(cc, title);
    connect(cc, &CircuitCollection::selectCircuit, this, &CircuitChooseDialog::accept);
}
void CircuitChooseDialog::setCurrentCircuit(QString name)
{
    for (qsizetype i=0; i<tabWidget->count(); i++) {
        CircuitCollection *cc = (CircuitCollection *)tabWidget->widget(i);
        if (cc->preselectCircuit(name)) {
            tabWidget->setCurrentIndex(i);
            break;
        }
    }
}
void CircuitChooseDialog::switchToNextCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() + 1) % tabWidget->count());
    if (tabWidget->currentIndex() == TAB_INDEX_SEARCH && lineEditSearch->text().isEmpty())
        switchToNextCategory();
}
void CircuitChooseDialog::switchToPreviousCategory()
{
    tabWidget->setCurrentIndex((tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count());
    if (tabWidget->currentIndex() == TAB_INDEX_SEARCH && lineEditSearch->text().isEmpty())
        switchToPreviousCategory();
}

void CircuitChooseDialog::switchToFirstCategory()
{
    tabWidget->setCurrentIndex(0);
    if (tabWidget->currentIndex() == TAB_INDEX_SEARCH && lineEditSearch->text().isEmpty())
        switchToNextCategory();
}

void CircuitChooseDialog::switchToLastCategory()
{
    tabWidget->setCurrentIndex(tabWidget->count() - 1);
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
void CircuitChooseDialog::showManual()
{
    the_manual->showCircuit(getSelectedCircuit());
}
void CircuitChooseDialog::keyPressed(int key)
{
    CircuitCollection *collection = (CircuitCollection *)tabWidget->currentWidget();

    switch (key) {
    case Qt::Key_Up:
        collection->moveCursorUpDown(-1);
        break;
    case Qt::Key_Down:
        collection->moveCursorUpDown(1);
        break;
    case Qt::Key_Left:
        switchToPreviousCategory();
        break;
    case Qt::Key_Right:
        switchToNextCategory();
        break;
    case Qt::Key_Home:
        switchToFirstCategory();
        break;
    case Qt::Key_End:
        switchToLastCategory();
        break;
    }
}
