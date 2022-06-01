#include "updatehub.h"

#include <QDebug>


UpdateHub *the_hub = 0;

UpdateHub::UpdateHub(QObject *parent)
    : QObject{parent}
{
    the_hub = this;
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

void UpdateHub::changeSelection(const Selection *selection)
{
    // Called when the range of the selection in the PatchSectionView
    // has changed.
    qDebug() << "HUB: changeSelection()";
    emit selectionChanged(selection);
}

void UpdateHub::moveCursor()
{
    // Called, when the cursor within the current section moved but
    // nothing else has changed.
    qDebug() << "HUB: cursorMoved()";
    emit cursorMoved();
}

void UpdateHub::changePatching()
{
    // Called when the editor state "patching" has changed
    qDebug() << "HUB: patchingChanged()";
    emit patchingChanged();
}
