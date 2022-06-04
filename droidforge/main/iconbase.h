#ifndef ICONBASE_H
#define ICONBASE_H

#include <QMap>
#include <QIcon>

class IconBase
{
    QMap<QString,QIcon> icons;
    const QIcon &getIcon(const QString &name);

public:
    static const QIcon &icon(const QString &name);
};

#define ICON(i) IconBase::icon(i)

#endif // ICONBASE_H
