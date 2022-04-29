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


void Circuit::deleteJackAssignment(unsigned i)
{
    delete jackAssignments[i];
    jackAssignments.remove(i);
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
