#ifndef ICONBASE_H
#define ICONBASE_H

#include <QMap>
#include <QIcon>
#include <QImage>

class IconBase
{
    QMap<QString,QIcon> icons;
    QMap<QString,QImage> images;
    const QIcon &getIcon(const QString &name);
    const QImage &getImage(const QString &name);

public:
    static const QIcon &icon(const QString &name);
    static const QImage &image(const QString &name);
};

#define ICON(i) IconBase::icon(i)
#define IMAGE(i) IconBase::image(i)

#endif // ICONBASE_H