#include "atomselectordialog.h"
#include "atomselector.h"

#include <QGridLayout>

AtomSelectorDialog::AtomSelectorDialog(QWidget *parent)
    : QDialog{parent}
{
    resize(600, 200);
    setWindowTitle(tr("Edit value"));

    // Special widget for selecting values
    atomSelector = new AtomSelector(this);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(atomSelector, 0, 0, 1, -1);
    mainLayout->addWidget(buttonBox, 1, 1);
    setLayout(mainLayout);
}


Atom *AtomSelectorDialog::editAtom(jacktype_t , const Atom *atom)
{
    // TODO: input / output unterscheiden
    atomSelector->setAtom(atom);

    if (exec() == QDialog::Accepted)
        return atomSelector->getAtom();
    else
        return const_cast<Atom *>(atom); // We know we haven't changed it
}
