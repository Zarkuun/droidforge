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
    // Called, when the central pointer MainWindow::patch has changed.
    // This is the case, when a new patch is loaded are "New..." is
    // selected.
    qDebug() << "HUB: changePatch()";
    emit patchChanged(patch);
}

void UpdateHub::modifyPatch()
{
    // Called, when the current patch is modified in any possible way -
    // except for (only) cursor movements. This is always done directly
    // after a patch->commit() from one of our subsystems.
    qDebug() << "HUB: modifyPatch()";
    emit patchModified();
}

void UpdateHub::switchSection()
{
    // Called whenever another patch section is now being selected.
    qDebug() << "HUB: switchSection()";
    emit sectionSwitched();
}

void UpdateHub::changeClipboard()
{
    // Called when the contents of the clipboard change
    qDebug() << "HUB: changeClipboard()";
    emit clipboardChanged();
}
