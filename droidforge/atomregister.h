#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QChar>

#include "atom.h"

#define    REGISTER_INPUT  'I'
#define    REGISTER_OUTPUT 'O'
#define    REGISTER_BUTTON 'B'
#define    REGISTER_LED    'L'
#define    REGISTER_POT    'P'
#define    REGISTER_SWITCH 'S'
#define    REGISTER_RGB    'R'
#define    REGISTER_EXTRA  'X'


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
};

#endif // ATOMREGISTER_H
