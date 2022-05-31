#include "updatehub.h"

#include <QDebug>


UpdateHub *the_hub = 0;

UpdateHub::UpdateHub(QObject *parent)
    : QObject{parent}
{
    the_hub = this;
}

void UpdateHub::changePatch(VersionedPatch *patch)
{
    qDebug() << "HUB: changePatch()";
    emit patchChanged(patch);
}

void UpdateHub::switchSection()
{
    qDebug() << "HUB: switchSection()";
    emit sectionSwitched();
}
