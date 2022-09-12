#include "iconbase.h"
#include "globals.h"
#include "tuning.h"
#include "colorscheme.h"

IconBase the_iconbase;

const QIcon &IconBase::getIcon(const QString &name)
{
    if (!icons.contains(name)) {
        QIcon icon(QString(ICON_PATH_TEMPLATE)
                   .arg(the_colorscheme->isDark() ? "dark" : "light",
                        name));
        icons[name] = icon;
    }
    return icons[name];
}
const QIcon &IconBase::icon(const QString &name)
{
    return the_iconbase.getIcon(name);
}
const QImage &IconBase::getImage(const QString &name)
{
    if (!images.contains(name)) {
        QImage image(QString(ICON_PATH_TEMPLATE)
                   .arg(the_colorscheme->isDark() ? "dark" : "light",
                        name));
        images[name] = image;
    }
    return images[name];
}

const QPixmap &IconBase::getPixmap(const QString &name)
{
    if (!pixmaps.contains(name)) {
        QPixmap pixmap(QString(ICON_PATH_TEMPLATE)
                   .arg(the_colorscheme->isDark() ? "dark" : "light",
                        name));
        pixmaps[name] = pixmap;
    }
    return pixmaps[name];
}
const QImage &IconBase::getJackTypeSymbol(const QString &name)
{
    if (!jackTypeSymbols.contains(name)) {
        QImage jackTypeSymbol(QString(JACK_TYPE_SYMBOLS_PATH_TEMPLATE)
                   .arg(the_colorscheme->isDark() ? "dark" : "light",
                        name));
        jackTypeSymbols[name] = jackTypeSymbol.scaledToHeight(JSEL_JACK_HEIGHT, Qt::SmoothTransformation);

    }
    return jackTypeSymbols[name];
}
void IconBase::flushCaches()
{
    icons.clear();
    pixmaps.clear();
    images.clear();
    jackTypeSymbols.clear();
}
const QImage &IconBase::image(const QString &name)
{
    return the_iconbase.getImage(name);
}
const QPixmap &IconBase::pixmap(const QString &name)
{
    return the_iconbase.getPixmap(name);
}
const QImage &IconBase::jackTypeSymbol(const QString &name)
{
    return the_iconbase.getJackTypeSymbol(name);

}
