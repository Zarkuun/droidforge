#include "circuit.h"

Circuit::~Circuit()
{
    for (qsizetype i=0; i<jackAssignments.length(); i++)
        delete jackAssignments[i];
}


Circuit *Circuit::clone() const
{
    Circuit *newcircuit = new Circuit(name);
    for (unsigned i=0; i<jackAssignments.size(); i++)
        newcircuit->jackAssignments.append(jackAssignments[i]->clone());
    return newcircuit;
}


void Circuit::addJackAssignment(JackAssignment *ja)
{
    jackAssignments.push_back(ja);

}


void Circuit::deleteJackAssignment(unsigned i)
{
    delete jackAssignments[i];
    jackAssignments.remove(i);
}


QString Circuit::toString()
{
    QString s;
    if (!comment.isEmpty()) {
        for (qsizetype i=0; i<comment.size(); i++)
             s += "# " + comment[i] + "\n"; // TODO Multiline
    }

    // TODO: disabled
    s += "[" + name + "]\n";

    for (qsizetype i=0; i<jackAssignments.length(); i++)
    {
        s += jackAssignments[i]->toString() + "\n";
    }
    s += "\n";
    return s;
}
