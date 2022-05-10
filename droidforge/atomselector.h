#ifndef ATOMSELECTOR_H
#define ATOMSELECTOR_H

#include "atom.h"
#include "numberselector.h"

#include <QWidget>

class AtomSelector : public QWidget
{
    Q_OBJECT
    NumberSelector *numberSelector;

public:
    explicit AtomSelector(QWidget *parent = nullptr);
    void setAtom(const Atom *atom);
    Atom *getAtom();

signals:

};

#endif // ATOMSELECTOR_H
