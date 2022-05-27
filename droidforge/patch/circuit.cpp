#include "circuit.h"

Circuit::Circuit(QString name, const QStringList &comment)
    : name(name)
    , comment(comment)
{
}


Circuit::~Circuit()
{
    for (qsizetype i=0; i<jackAssignments.length(); i++)
        delete jackAssignments[i];
}


Circuit *Circuit::clone() const
{
    Circuit *newcircuit = new Circuit(name, comment);
    for (unsigned i=0; i<jackAssignments.size(); i++)
        newcircuit->jackAssignments.append(jackAssignments[i]->clone());
    return newcircuit;
}


void Circuit::addJackAssignment(JackAssignment *ja)
{
    jackAssignments.append(ja);

}

void Circuit::insertJackAssignment(JackAssignment *ja, int index)
{
    jackAssignments.insert(index, ja);
}


void Circuit::deleteJackAssignment(unsigned i)
{
    delete jackAssignments[i];
    jackAssignments.remove(i);
}

const Atom *Circuit::atomAt(int row, int column) const
{
    return jackAssignments[row]->atomAt(column);
}

Atom *Circuit::atomAt(int row, int column)
{
    return jackAssignments[row]->atomAt(column);
}

void Circuit::collectCables(QStringList &cables) const
{
    for (auto ja: jackAssignments)
        ja->collectCables(cables);
}

void Circuit::findCableConnections(const QString &cable, int &asInput, int &asOutput) const
{
    for (auto ja: jackAssignments)
        ja->findCableConnections(cable, asInput, asOutput);
}

void Circuit::changeCircuit(QString newCircuit)
{
    name = newCircuit;
    QList<JackAssignment *> newJacks;
    for (qsizetype i=0; i<jackAssignments.length(); i++) {
        QString asString = jackAssignments[i]->toString();
        delete jackAssignments[i];
        JackAssignment *ja = JackAssignment::parseJackLine(newCircuit, asString);
        newJacks.append(ja);
    }
    jackAssignments.clear();
    jackAssignments = newJacks;
}

bool Circuit::needG8() const
{
    for (qsizetype i=0; i<jackAssignments.length(); i++) {
        if (jackAssignments[i]->needG8())
            return true;
    }
    return false;
}

bool Circuit::needX7() const
{
    // TODO: This seems somewhat hacky...
    if (name.startsWith("midi") || name == "firefacecontrol")
        return true;
    for (qsizetype i=0; i<jackAssignments.length(); i++) {
        if (jackAssignments[i]->needX7())
            return true;
    }
    return false;
}

void Circuit::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (auto ja: jackAssignments)
        ja->swapControllerNumbers(fromNumber, toNumber);
}

void Circuit::shiftControllerNumbers(int number, int by)
{
    for (auto ja: jackAssignments)
        ja->shiftControllerNumbers(number, by);
}

void Circuit::collectRegisterAtoms(RegisterList &sl) const
{
    for (auto ja: jackAssignments)
        ja->collectRegisterAtoms(sl);
}

void Circuit::remapRegister(AtomRegister from, AtomRegister to)
{
    for (auto ja: jackAssignments)
        ja->remapRegister(from, to);
}

void Circuit::removeRegisterReferences(RegisterList &rl, int ih, int oh)
{
    for (auto ja: jackAssignments)
        ja->removeRegisterReferences(rl, ih, oh);

}

QString Circuit::toString()
{
    QString s;
    for (qsizetype i=0; i<comment.size(); i++)
        if (comment[i].isEmpty())
             s += "\n";
         else
             s += "# " + comment[i] + "\n"; // TODO Multiline

    // TODO: disabled
    s += "[" + name + "]\n";

    for (qsizetype i=0; i<jackAssignments.length(); i++)
    {
        s += jackAssignments[i]->toString() + "\n";
    }
    s += "\n";
    return s;
}

QString Circuit::getComment() const
{
    if (comment.empty())
        return "";
    else
        return comment.join('\n') + "\n";
}

void Circuit::setComment(QString c)
{
    comment = c.trimmed().split("\n");
}

void Circuit::removeComment()
{
    comment.clear();
}
