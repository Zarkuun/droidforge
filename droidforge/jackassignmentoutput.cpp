#include "jackassignmentoutput.h"

JackAssignmentOutput::JackAssignmentOutput(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , atom(0)
{
    parseOutputValue(valueString);
}


JackAssignmentOutput::JackAssignmentOutput(QString jack, QString comment)
    : JackAssignment(jack, comment)
    , atom(0)
{
}


JackAssignmentOutput::~JackAssignmentOutput()
{
    if (atom) delete atom;
}

Atom *JackAssignmentOutput::getAtom() const
{
    return atom;
}


JackAssignment *JackAssignmentOutput::clone() const
{
    JackAssignmentOutput *newas = new JackAssignmentOutput(jack, comment);
    newas->disabled = disabled;
    if (atom)
        newas->atom = atom->clone();
    return newas;
}


QString JackAssignmentOutput::valueToString() const
{
    if (atom)
        return atom->toString();
    else
        return "";
}

void JackAssignmentOutput::replaceAtom(int, Atom *newAtom)
{
    if (atom)
        delete atom;
    atom = newAtom;
}


void JackAssignmentOutput::parseOutputValue(QString valueString)
{
    if (valueString.size() > 0) {
        if (valueString[0] == '_')
            atom = parseCable(valueString);
        else
            atom = parseRegister(valueString);
    }
}
