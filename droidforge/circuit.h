#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"

#include <QList>
#include <QString>

class Circuit
{
public:
    QString name;
    QString comment;
    bool disabled;
    QList<JackAssignment> jackAssignments;
};

#endif // CIRCUIT_H
