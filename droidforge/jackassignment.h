#ifndef JACKASSIGNMENT_H
#define JACKASSIGNMENT_H

#include <QString>

typedef enum {
    JACKTYPE_INPUT = 0,
    JACKTYPE_OUTPUT = 1,
    JACKTYPE_INVALID = 2
} jacktype_t;

class JackAssignment
{
public:
    QString jack;
    jacktype_t jackType;
    QString value; // TODO: split up?
    QString comment;
    bool disabled;
};

#endif // JACKASSIGNMENT_H
