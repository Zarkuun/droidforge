#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QChar>

#include "atom.h"
#include "registertypes.h"

class AtomRegister : public Atom
{
    union {
        struct {
            char registerType;
            uint8_t controller; // none: master,x7,g8,internal
            uint8_t number;
            uint8_t reserved;
        } r;
        uint32_t raw;
    } data;

public:
    AtomRegister(QChar t, unsigned c, unsigned n);
    AtomRegister(uint32_t raw);
    AtomRegister(const AtomRegister& ar);
    AtomRegister operator=(const AtomRegister &ar);
    AtomRegister *clone() const;
    QString toString() const;
    bool isRegister() const { return true; };
    bool isControl() { return data.r.controller > 0; };

    QChar getRegisterType() const { return data.r.registerType; };
    unsigned getController() const { return data.r.controller; };
    unsigned getNumber() const { return data.r.number; };
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromindex, int toindex);
};

typedef QList<AtomRegister> RegisterList;


#endif // ATOMREGISTER_H
