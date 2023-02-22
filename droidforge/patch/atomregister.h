#ifndef ATOMREGISTER_H
#define ATOMREGISTER_H

#include <QDataStream>
#include <QList>

#include "atom.h"
#include "registertypes.h"

class AtomRegister : public Atom
{
    register_type_t registerType;
    uint8_t cont; // none: master,x7,g8,internal
    uint8_t num;

public:
    AtomRegister();
    AtomRegister(char t, unsigned c, unsigned n);
    AtomRegister(const AtomRegister& ar);
    AtomRegister(const QString &s);
    AtomRegister operator=(const AtomRegister &ar);
    bool isNull() const;
    AtomRegister relatedRegisterWithLabel() const;
    bool isControl() { return cont > 0; };
    bool isOnMasterG8X7() { return cont == 0; };
    unsigned controller() const { return cont; };
    unsigned number() const { return num; };
    unsigned getController() const { return cont; }
    unsigned getNumber() const { return num; }
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
    bool needsG8() const override;
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
