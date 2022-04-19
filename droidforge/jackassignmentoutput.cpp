#include "jackassignmentoutput.h"
#include "QtCore/qdebug.h"

JackAssignmentOutput::JackAssignmentOutput(QString jack, QString valueString)
    : JackAssignment(jack)
    , atom(0)
{
    parseOutputValue(valueString);
}


JackAssignmentOutput::JackAssignmentOutput(QString jack)
    : JackAssignment(jack)
    , atom(0)
{

}


JackAssignmentOutput::~JackAssignmentOutput()
{
    if (atom) delete atom;
}

Atom *JackAssignmentOutput::getAtom() const
{
    qDebug() << "ATOM";
    return atom;
}


JackAssignment *JackAssignmentOutput::clone() const
{
    JackAssignmentOutput *newas = new JackAssignmentOutput(jack);
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


void JackAssignmentOutput::parseOutputValue(QString valueString)
{
    if (valueString.size() > 0) {
        if (valueString[0] == '_')
            atom = parseCable(valueString);
        else
            atom = parseRegister(valueString);
    }
}
