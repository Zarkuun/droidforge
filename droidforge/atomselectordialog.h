#ifndef ATOMSELECTORDIALOG_H
#define ATOMSELECTORDIALOG_H

#include "jackassignment.h"
#include "patch.h"

#include <QDialog>
#include <QDialogButtonBox>

class AtomSelector;

class AtomSelectorDialog : public QDialog
{
    AtomSelector *atomSelector;
    QDialogButtonBox *buttonBox;

public:
    AtomSelectorDialog(QWidget *parent = nullptr);
    Atom *editAtom(const Patch *patch, jacktype_t, const Atom *atom);
    bool eventFilter(QObject *o, QEvent *e);
};

#endif // ATOMSELECTORDIALOG_H
