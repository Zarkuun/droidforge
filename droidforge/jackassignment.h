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
private:
    // TODO: Das hier wieder weg, wenn alles geht
    JackAssignment(JackAssignment &ja);
    JackAssignment(const JackAssignment &ja);

public:
    QString jack;
    jacktype_t jackType;
    QString comment;
    bool garbled;
    bool disabled;
    Atom *atomA; // mult 1
    Atom *atomB; // mult 2
    Atom *atomC; // add
    QString sourceString; // in case of garbled

public:
    JackAssignment();
    ~JackAssignment();
    QString toString();
    void parseSourceString(QString s);

private:
    void parseInputValue();
    void parseOutputValue();
    Atom *parseInputAtom(QString s);
    Atom *parseCable(QString s);
    Atom *parseRegister(QString s);
    Atom *parseOnOff(QString s);
    Atom *parseNumber(QString s);
};

#endif // JACKASSIGNMENT_H
