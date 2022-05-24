#ifndef ATOMONELINER_H
#define ATOMONELINER_H

#include "atom.h"
#include "dialog.h"
#include "patch.h"

#include <QLineEdit>
#include <QPoint>

class AtomOneliner : public Dialog
{
    Q_OBJECT
    QLineEdit *lineEdit;
    jacktype_t jacktype;
    bool initialDeselect;

    AtomOneliner(QWidget *parent = nullptr);
    void returnPressed();
    void selectionChanged();
    bool edit(QPoint dialogPos, jacktype_t jacktype, QString start);

public:
    static Atom *editAtom(QPoint dialogPos, const Patch *patch, jacktype_t, QString start);
    Atom *getAtom();
};

#endif // ATOMONELINER_H
