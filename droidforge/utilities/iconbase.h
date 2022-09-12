#ifndef ICONBASE_H
#define ICONBASE_H

#include <QMap>
#include <QIcon>
#include <QImage>
#include <QPixmap>

class IconBase
{
    QMap<QString,QIcon> icons;
    QMap<QString,QImage> images;
    QMap<QString,QPixmap> pixmaps;
    QMap<QString,QImage> jackTypeSymbols;

    const QIcon &getIcon(const QString &name);
    const QImage &getImage(const QString &name);
    const QPixmap &getPixmap(const QString &name);
    const QImage &getJackTypeSymbol(const QString &name);

public:
    void flushCaches();
    static const QIcon &icon(const QString &name);
    static const QImage &image(const QString &name);
    static const QPixmap &pixmap(const QString &name);
    static const QImage &jackTypeSymbol(const QString &name);
};

extern IconBase the_iconbase;

#define ICON(i) IconBase::icon(i)
#define IMAGE(i) IconBase::image(i)
#define PIXMAP(i) IconBase::pixmap(i)

#endif // ICONBASE_H
