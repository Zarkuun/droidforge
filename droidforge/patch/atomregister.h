#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QDataStream>
#include <QList>

#include "atom.h"
#include "registertypes.h"

class AtomRegister : public Atom
{
    register_type_t registerType;
    uint8_t controller; // 0: master, x7, internal, R-registers of G8
    uint8_t g8; // 1, 2, 3 or 4 for a G8
    uint8_t number; // The 4 in "P1.4". Or the 8 in "I8"

public:
    AtomRegister();
    AtomRegister(char ty, unsigned co, unsigned g8, unsigned nr);
    AtomRegister(const AtomRegister& ar);
    AtomRegister(const QString &s);
    AtomRegister operator=(const AtomRegister &ar);
    bool isNull() const;
    AtomRegister relatedRegisterWithLabel() const;
    bool isControl() { return controller > 0; };
    unsigned getController() const { return controller; }
    unsigned getNumber() const { return number; }
    bool isRelatedTo(const AtomRegister &other) const;

    void setRegisterType(register_type_t t) { registerType = t; };
    register_type_t getRegisterType() const { return registerType; };
    void swapControllerNumbers(int fromController, int toController);
    void shiftControllerNumbers(int controller, int by);

    bool canHaveLabel() const override;
    bool isRegister() const  override { return true; };
    AtomRegister *clone() const override;
    QString toString() const override;
    QString toDisplay() const override;
    unsigned neededG8Number() const override;
    bool needsX7() const override;
    QString problemAsInput(const Patch *patch) const override;
    QString problemAsOutput(const Patch *patch) const override;
    void incrementForExpansion(const Patch *patch) override;

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
