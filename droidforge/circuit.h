#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"

#include <QList>
#include <QString>
#include <QStringList>

class Circuit
{
public:
    QString name;
    QStringList comment;
    bool disabled;
    QList<JackAssignment> jackAssignments;

    QString toString();
};

#endif // CIRCUIT_H
