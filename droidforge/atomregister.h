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
public:
    QChar registerType;
    unsigned controller; // 0: none
    unsigned number;
    AtomRegister(QChar t, unsigned c, unsigned n)
        : registerType(t), controller(c), number(n) {};
    QString toString();
};

#endif // ATOMREGISTER_H