#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"

#include <QList>
#include <QString>
#include <QStringList>

class Circuit
{
    Circuit(Circuit &circuit);
    Circuit(const Circuit &circuit);
    QList<JackAssignment *> jackAssignments;

public:
    Circuit(QString name) : name(name) {};
    ~Circuit();
    void addJackAssignment(JackAssignment *);
    JackAssignment *jackAssignment(unsigned i) { return jackAssignments[i]; };
    qsizetype numJackAssignments() {  return jackAssignments.count(); };
    void deleteJackAssignment(unsigned i);

    QString name;
    QStringList comment;
    bool disabled;

    QString toString();
};

#endif // CIRCUIT_H
