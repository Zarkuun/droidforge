#include "jackassignmentoutput.h"
#include "atomcable.h"
#include "atominvalid.h"

JackAssignmentOutput::JackAssignmentOutput(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
{
    atom = parseOutputAtom(valueString);
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

void JackAssignmentOutput::collectCables(QStringList &cables) const
{
    if (atom && atom->isCable())
        cables.append(((AtomCable *)atom)->getCable());
}

void JackAssignmentOutput::parseExpression(const QString &expression)
{
    atom = parseOutputAtom(expression);
}

Atom *JackAssignmentOutput::parseOutputAtom(const QString &expression)
{
    Atom *atom = 0;
    if (expression.size() > 0) {
        if (expression[0] == '_')
            atom = parseCable(expression);
        else
            atom = parseRegister(expression);
    }
    if (atom)
        return atom;
    else
        return new AtomInvalid(expression);
}
