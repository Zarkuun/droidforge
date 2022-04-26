#ifndef REGISTERCOMMENTS_H
#define REGISTERCOMMENTS_H

#include "atomregister.h"

#include <QString>
#include <QList>

struct RegisterComment {
    AtomRegister *atom;
    QString shorthand;
    QString comment;
};

class RegisterComments
{
    QList <RegisterComment> comments;

public:
    RegisterComments() {};
    ~RegisterComments();
    void addComment(AtomRegister *atom, QString shorthand, QString comment);
};

#endif // REGISTERCOMMENTS_H
