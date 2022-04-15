#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"

#include <QList>
#include <QString>
#include <QStringList>

class Circuit
{
    QList<JackAssignment *> jackAssignments;

public:
    Circuit(QString name) : name(name) {};
    ~Circuit();
    Circuit *clone() const;
    QString toString();
    void addJackAssignment(JackAssignment *);
    JackAssignment *jackAssignment(unsigned i) { return jackAssignments[i]; };
    qsizetype numJackAssignments() {  return jackAssignments.count(); };
    void deleteJackAssignment(unsigned i);

    QString name;
    QStringList comment;
    bool disabled;

};

#endif // CIRCUIT_H
