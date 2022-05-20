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
    RegisterComments *clone() const;
    void addComment(AtomRegister *atom, QString shorthand, QString comment);
    QString toString() const;

private:
    QString toString(char reg, unsigned controller, const char *title=0) const;
};

#endif // REGISTERCOMMENTS_H
