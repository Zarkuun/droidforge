#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QChar>
#include <QDataStream>
#include <QList>

#include "atom.h"
#include "registertypes.h"

class AtomRegister : public Atom
{
    char registerType;
    uint8_t cont; // none: master,x7,g8,internal
    uint8_t num;

public:
    AtomRegister();
    AtomRegister(char t, unsigned c, unsigned n);
    AtomRegister(QChar t, unsigned c, unsigned n); // TODO: Get rid of this crap
    AtomRegister(const AtomRegister& ar);
    AtomRegister operator=(const AtomRegister &ar);
    AtomRegister *clone() const;
    QString toString() const;
    QString toDisplay() const;
    bool isRegister() const { return true; };
    bool isControl() { return cont > 0; };
    bool isOnMasterG8X7() { return cont == 0; };
    unsigned controller() const { return cont; };
    unsigned number() const { return num; };
    unsigned getController() const { return cont; }
    unsigned getNumber() const { return num; }

    void setRegisterType(QChar t) { registerType = t.toLatin1(); };
    QChar getRegisterType() const { return registerType; };
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
bool operator!=(const AtomRegister &a, const AtomRegister &b);
bool operator==(const AtomRegister &a, const AtomRegister &b);
bool operator<(const AtomRegister &a, const AtomRegister &b);

#endif // ATOMREGISTER_H
