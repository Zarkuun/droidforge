#include "iconbase.h"
#include "globals.h"
#include "tuning.h"

IconBase the_iconbase;

const QIcon &IconBase::getIcon(const QString &name)
{
    if (!icons.contains(name)) {
        QIcon icon(QString(ICON_PATH_TEMPLATE).arg(name));
        icons[name] = icon;
    }
    return icons[name];
}
const QIcon &IconBase::icon(const QString &name)
{
    return the_iconbase.getIcon(name);
}
