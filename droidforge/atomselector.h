#ifndef ATOMSELECTOR_H
#define ATOMSELECTOR_H

#include "atom.h"
#include "numberselector.h"
#include "inputoutputselector.h"

#include <QWidget>

class AtomSelector : public QWidget
{
    Q_OBJECT
    NumberSelector *numberSelector;
    InputOutputSelector *inputOutputSelector;

public:
    explicit AtomSelector(QWidget *parent = nullptr);
    void setAtom(const Atom *atom);
    Atom *getAtom();

signals:

private:
    QWidget *vline() const;
    QWidget *hline() const;

};

#endif // ATOMSELECTOR_H
