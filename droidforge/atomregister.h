#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QChar>

#include "atom.h"
#include "registertypes.h"

// TODO: use these in the Module... classes

class AtomRegister : public Atom
{
    QChar registerType;
    unsigned controller; // 0: none
    unsigned number;

public:
    AtomRegister(QChar t, unsigned c, unsigned n)
        : registerType(t), controller(c), number(n) {};
    AtomRegister *clone() const;
    QString toString() const;
    bool isRegister() const { return true; };
    bool isControl() { return controller > 0; };

    QChar getRegisterType() const { return registerType; };
    unsigned getController() const { return controller; };
    unsigned getNumber() const { return number; };
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromindex, int toindex);
    void collectRegisterAtoms(QStringList &) const;
};

#endif // ATOMREGISTER_H
