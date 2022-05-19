#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QChar>
#include <QDataStream>
#include <QList>

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
    AtomRegister();
    AtomRegister(QChar t, unsigned c, unsigned n);
    AtomRegister(uint32_t raw);
    AtomRegister(const AtomRegister& ar);
    AtomRegister operator=(const AtomRegister &ar);
    AtomRegister *clone() const;
    QString toString() const;
    bool isRegister() const { return true; };
    bool isControl() { return data.r.controller > 0; };

    void setRegisterType(QChar t) { data.r.registerType = t.toLatin1(); };
    QChar getRegisterType() const { return data.r.registerType; };
    unsigned getController() const { return data.r.controller; };
    unsigned getNumber() const { return data.r.number; };
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromindex, int toindex);
    friend bool operator==(const AtomRegister &a, const AtomRegister &b);
};

typedef QList<AtomRegister> RegisterList;

QDebug &operator<<(QDebug &out, const AtomRegister &ar);
QDebug &operator<<(QDebug &out, const RegisterList &rl);
bool operator==(const AtomRegister &a, const AtomRegister &b);

#endif // ATOMREGISTER_H
