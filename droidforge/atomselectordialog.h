#ifndef ATOMSELECTORDIALOG_H
#define ATOMSELECTORDIALOG_H

#include "jackassignment.h"
#include "patch.h"
#include "dialog.h"

#include <QDialog>
#include <QDialogButtonBox>

class AtomSelector;

class AtomSelectorDialog : public Dialog
{
    AtomSelector *atomSelector;
    QDialogButtonBox *buttonBox;

public:
    AtomSelectorDialog(jacktype_t, QWidget *parent = nullptr);
    static Atom *editAtom(const Patch *patch, jacktype_t, const Atom *atom);
    // void keyPressEvent(QKeyEvent *event);
    // bool eventFilter(QObject *o, QEvent *e);

};

#endif // ATOMSELECTORDIALOG_H
