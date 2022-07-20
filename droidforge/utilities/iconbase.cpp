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
        QImage image(QString(IMAGE_PATH_TEMPLATE).arg(name));
        images[name] = image;
    }
    return images[name];
}

const QImage &IconBase::image(const QString &name)
{
    return the_iconbase.getImage(name);
}
