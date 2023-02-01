#include "jackassignmentoutput.h"
#include "atomcable.h"
#include "atominvalid.h"

#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

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

void JackAssignmentOutput::findCableConnections(const QString &cable, int&, int &asOutput) const
{
    if (atom && atom->isCable() && ((AtomCable *)atom)->getCable() == cable)
        asOutput ++;
}

void JackAssignmentOutput::parseExpression(const QString &expression)
{
    atom = parseOutputAtom(expression);
}

Atom *JackAssignmentOutput::parseOutputAtom(const QString &expression)
{
    if (expression == "")
        return 0;

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

void JackAssignmentOutput::removeRegisterReferences(RegisterList &rl)
{
    if (atom && atom->isRegister()) {
        AtomRegister *ar = (AtomRegister *)atom;
        if (rl.contains(*ar)) {
            delete atom;
            atom = 0;
        }
    }
}

QList<PatchProblem *> JackAssignmentOutput::collectProblems(const Patch *patch) const
{
    QList<PatchProblem *>problems;
    if (!atom)
        problems.append(new PatchProblem(-1, 1, tr("You need to set a value for this parameter")));
    else {
        QString text = atom->problemAsOutput(patch);
        if (text != "")
            problems.append(new PatchProblem(-1, 1, text));
    }
    return problems;
}
