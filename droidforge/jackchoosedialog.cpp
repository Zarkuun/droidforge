#include "jackchoosedialog.h"
#include "jackselector.h"
#include "droidfirmware.h"

#include <QGridLayout>
#include <QLabel>
#include <QKeyEvent>

JackChooseDialog::JackChooseDialog(QWidget *parent)
    : Dialog{"jackchooser", parent}
{
    setWindowTitle(tr("Add new jack / parameter"));

    // Canvas with circuit diagram
    jackSelector = new JackSelector(this);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Search
    QLabel *searchLabel = new QLabel(tr("Search:"), this);
    lineEditSearch = new QLineEdit(this);
    connect(lineEditSearch, &QLineEdit::textChanged, jackSelector, &JackSelector::searchChanged);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(jackSelector, 0, 0, 1, -1);
    mainLayout->addWidget(searchLabel, 1, 0);
    mainLayout->addWidget(lineEditSearch, 1, 1);
    mainLayout->addWidget(buttonBox, 1, 2);
    setLayout(mainLayout);

    connect(jackSelector, &JackSelector::cursorMoved, this, &JackChooseDialog::cursorMoved);
}


void JackChooseDialog::setCircuit(const QString &circuit, const QString &current, const QStringList &usedJacks, jacktype_t jackType)
{
    jackSelector->setCircuit(circuit, current, usedJacks, jackType, lineEditSearch->text());
}


QString JackChooseDialog::getSelectedJack() const
{
    return jackSelector->getSelectedJack();
}


void JackChooseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
        lineEditSearch->insert(event->text());
    else if (event->key() == Qt::Key_Backspace)
        lineEditSearch->backspace();
    else
        QDialog::keyPressEvent(event);
}


QString JackChooseDialog::chooseJack(const QString &circuit, const QString &current, const QStringList &used, jacktype_t jackType)
{
    static JackChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new JackChooseDialog();

    dialog->setCircuit(circuit, current, used, jackType);
    if (dialog->exec() == QDialog::Accepted)
        return dialog->getSelectedJack();
    else
        return "";
}


void JackChooseDialog::cursorMoved(bool onActive)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(onActive);

}
