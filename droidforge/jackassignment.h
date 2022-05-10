#ifndef JACKASSIGNMENT_H
#define JACKASSIGNMENT_H

#include "atom.h"
#include <QString>

typedef enum {
    JACKTYPE_INPUT = 0,
    JACKTYPE_OUTPUT = 1,
    JACKTYPE_UNKNOWN = 2
} jacktype_t;


class JackAssignment
{

protected:
    QString jack;
    QString comment;
    bool disabled;

public:
    JackAssignment(QString jack, QString comment = "");
    virtual ~JackAssignment();
    virtual JackAssignment *clone() const = 0;
    virtual jacktype_t jackType() const = 0;
    QString toString() const;
    QString jackName() const { return jack; };
    virtual const Atom *atomAt(int column) const = 0;
    virtual void replaceAtom(int column, Atom *newAtom) = 0;

protected:
    virtual QString valueToString() const = 0;
    Atom *parseCable(QString s);
    Atom *parseRegister(QString s);
};

#endif // JACKASSIGNMENT_H
