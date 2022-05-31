#ifndef REGISTERCOMMENTS_H
#define REGISTERCOMMENTS_H

#include "atomregister.h"

#include <QString>
#include <QList>

struct RegisterComment {
    AtomRegister atom;
    QString shorthand;
    QString comment;
    // RegisterComment(AtomRegister a, const QString &s, const QString &c) : atom(a), shorthand(s), comment(c) {};
};

class RegisterComments : public QList<RegisterComment>
{
public:
    QString toString() const;

private:
    QString toString(char reg, unsigned controller, const QString &title=0) const;
};

#endif // REGISTERCOMMENTS_H
