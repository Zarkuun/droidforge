#include "atomselectordialog.h"
#include "atomselector.h"
#include "usermanual.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QKeyEvent>

AtomSelectorDialog::AtomSelectorDialog(jacktype_t jacktype, QWidget *parent)
    : Dialog{"atomselector", parent}
{
    if (jacktype == JACKTYPE_INPUT)
        setWindowTitle(tr("Edit input parameter"));
    else
        setWindowTitle(tr("Edit output parameter"));

    // Special widget for selecting values
    atomSelector = new AtomSelector(jacktype, this);
    connect(atomSelector, &AtomSelector::comitted, this, &QDialog::accept);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *manualButton = new QPushButton(tr("Manual"));
    connect(manualButton, &QPushButton::clicked, this, &AtomSelectorDialog::showManual);
    buttonBox->addButton(manualButton, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(atomSelector);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}
void AtomSelectorDialog::setAllowFraction(bool allowFraction)
{
    atomSelector->setAllowFraction(allowFraction);
}
void AtomSelectorDialog::setCircuitAndJack(QString circuit, QString jack)
{
    atomSelector->setCircuitAndJack(circuit, jack);
    this->circuit = circuit;
}
Atom *AtomSelectorDialog::editAtom(const Patch *patch, const QString &circuit, const QString &jack, jacktype_t jacktype, bool allowFraction, const Atom *atom)
{
    static AtomSelectorDialog *inputAtomSelectorDialog = 0;
    static AtomSelectorDialog *outputAtomSelectorDialog = 0;

    AtomSelectorDialog *dialog;

    if (jacktype == JACKTYPE_INPUT) {
        if (!inputAtomSelectorDialog)
            inputAtomSelectorDialog = new AtomSelectorDialog(JACKTYPE_INPUT);
        inputAtomSelectorDialog->setAllowFraction(allowFraction);
        inputAtomSelectorDialog->setCircuitAndJack(circuit, jack);
        dialog = inputAtomSelectorDialog;
    }
    else {
        if (!outputAtomSelectorDialog)
            outputAtomSelectorDialog = new AtomSelectorDialog(JACKTYPE_OUTPUT);
        outputAtomSelectorDialog->setCircuitAndJack(circuit, jack);
        dialog = outputAtomSelectorDialog;
    }

    dialog->atomSelector->setPatch(patch);
    dialog->atomSelector->setAtom(patch, atom);
    int result = dialog->exec();
    if (result == QDialog::Accepted)
        return dialog->atomSelector->getAtom();
    else
        return const_cast<Atom *>(atom); // We know we haven't changed it
}
void AtomSelectorDialog::showManual()
{
    the_manual->showCircuit(circuit);
}
