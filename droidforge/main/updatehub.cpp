#include "updatehub.h"

UpdateHub *the_hub = 0;

UpdateHub::UpdateHub(QObject *parent)
    : QObject{parent}
{
    the_hub = this;
}

void UpdateHub::changePatch()
{
    emit patchChanged();
}

void UpdateHub::switchSection()
{
    emit sectionSwitched();
}
