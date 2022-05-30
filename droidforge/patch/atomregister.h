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
    QString toDisplay() const;
    bool isRegister() const { return true; };
    bool isControl() { return data.r.controller > 0; };
    bool isOnMasterG8X7() { return data.r.controller == 0; };

    unsigned controller() const { return data.r.controller; };
    unsigned number() const { return data.r.number; };

    void setRegisterType(QChar t) { data.r.registerType = t.toLatin1(); };
    QChar getRegisterType() const { return data.r.registerType; };
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromController, int toController);
    void shiftControllerNumbers(int controller, int by);
    QString problemAsInput(const Patch *patch) const;
    QString problemAsOutput(const Patch *patch) const;

    friend bool operator==(const AtomRegister &a, const AtomRegister &b);
    friend bool operator<(const AtomRegister &a, const AtomRegister &b);

private:
    QString generalProblem(const Patch *patch) const;
};

QDebug &operator<<(QDebug &out, const AtomRegister &ar);
bool operator==(const AtomRegister &a, const AtomRegister &b);
bool operator<(const AtomRegister &a, const AtomRegister &b);

#endif // ATOMREGISTER_H
