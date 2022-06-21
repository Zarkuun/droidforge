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
    void setAllowFraction(bool allowFraction);
    void setCircuitAndJack(QString circuit, QString jack);
    static Atom *editAtom(const Patch *patch, const QString &circuit, const QString &jack, jacktype_t, bool allowFraction, const Atom *atom);
    // void keyPressEvent(QKeyEvent *event);
    // bool eventFilter(QObject *o, QEvent *e);

};

#endif // ATOMSELECTORDIALOG_H
