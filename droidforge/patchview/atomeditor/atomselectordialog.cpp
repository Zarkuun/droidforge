#include "atomselectordialog.h"
#include "atomselector.h"

#include <QGridLayout>
#include <QKeyEvent>

AtomSelectorDialog::AtomSelectorDialog(jacktype_t jacktype, QWidget *parent)
    : Dialog{"atomselector", parent}
{
    if (jacktype == JACKTYPE_INPUT)
        setWindowTitle(tr("Edit value for input jack"));
    else
        setWindowTitle(tr("Edit value for output jack"));

    // Special widget for selecting values
    atomSelector = new AtomSelector(jacktype, this);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(atomSelector, 0, 0, 1, -1);
    mainLayout->addWidget(buttonBox, 1, 1);
    setLayout(mainLayout);
}


// static
Atom *AtomSelectorDialog::editAtom(const Patch *patch, jacktype_t jacktype, const Atom *atom)
{
    static AtomSelectorDialog *inputAtomSelectorDialog = 0;
    static AtomSelectorDialog *outputAtomSelectorDialog = 0;

    AtomSelectorDialog *dialog;

    if (jacktype == JACKTYPE_INPUT) {
        if (!inputAtomSelectorDialog)
            inputAtomSelectorDialog = new AtomSelectorDialog(JACKTYPE_INPUT);
        dialog = inputAtomSelectorDialog;
    }
    else {
        if (!outputAtomSelectorDialog)
            outputAtomSelectorDialog = new AtomSelectorDialog(JACKTYPE_OUTPUT);
        dialog = outputAtomSelectorDialog;
    }

    dialog->atomSelector->setAtom(patch, atom);
    int result = dialog->exec();
    if (result == QDialog::Accepted)
        return dialog->atomSelector->getAtom();
    else
        return const_cast<Atom *>(atom); // We know we haven't changed it
}