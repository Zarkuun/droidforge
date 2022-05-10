#ifndef ATOMSELECTORDIALOG_H
#define ATOMSELECTORDIALOG_H

#include "jackassignment.h"

#include <QDialog>
#include <QDialogButtonBox>

class AtomSelector;

class AtomSelectorDialog : public QDialog
{
    AtomSelector *atomSelector;
    QDialogButtonBox *buttonBox;

public:
    AtomSelectorDialog(QWidget *parent = nullptr);
    Atom *editAtom(jacktype_t, const Atom *atom);
};

#endif // ATOMSELECTORDIALOG_H
