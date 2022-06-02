#ifndef REGISTERLABELS_H
#define REGISTERLABELS_H

#include "atomregister.h"

#include <QString>
#include <QList>
#include <QMap>

struct RegisterLabel {
    AtomRegister atom;
    QString shorthand;
    QString description;
    // RegisterComment(AtomRegister a, const QString &s, const QString &c) : atom(a), shorthand(s), comment(c) {};
};

class RegisterLabels : public QMap<AtomRegister, RegisterLabel>
{
public:
    QString toString() const;

private:
    QString toString(char reg, unsigned controller, const QString &title=0) const;
};

#endif // REGISTERLABELS_H
