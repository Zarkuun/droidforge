#ifndef REGISTERLIST_H
#define REGISTERLIST_H

#include "atomregister.h"

#include <QList>
#include <QDebug>

class RegisterList : public QList<AtomRegister>
{
public:
    RegisterList();
    QString toString() const;
};

QDebug &operator<<(QDebug &out, const RegisterList &rl);


#endif // REGISTERLIST_H
