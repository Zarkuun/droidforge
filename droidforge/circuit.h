#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"

#include <QList>
#include <QString>
#include <QStringList>

class Circuit
{
    QString name;
    QStringList comment;
    QList<JackAssignment *> jackAssignments;
    bool disabled;

public:
    Circuit(QString name, const QStringList &comment);
    ~Circuit();
    Circuit *clone() const;
    QString toString();
    QString getName() const { return name; };
    void addJackAssignment(JackAssignment *);
    JackAssignment *jackAssignment(unsigned i) { return jackAssignments[i]; };
    qsizetype numJackAssignments() const { return jackAssignments.count(); };
    void deleteJackAssignment(unsigned i);

};

#endif // CIRCUIT_H
