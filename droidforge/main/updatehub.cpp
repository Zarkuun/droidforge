#include "updatehub.h"
#include "globals.h"

UpdateHub::UpdateHub(QObject *parent)
    : QObject{parent}
{
}

void UpdateHub::modifyPatch()
{
    // Called, when the current patch is modified in any possible way -
    // except for (only) cursor movements. This is always done directly
    // after a patch->commit() from one of our subsystems.
    emit patchModified();
}
void UpdateHub::switchSection()
{
    // Called whenever another patch section is now being selected.
    emit sectionSwitched();
}
void UpdateHub::changeSelection()
{
    // Called when the range of the selection in the PatchSectionView
    // has changed.
    emit selectionChanged();
}
void UpdateHub::moveCursor()
{
    // Called, when the cursor within the current section moved but
    // nothing else has changed.
    emit cursorMoved();
}
void UpdateHub::changePatching()
{
    // Called when the editor state "patching" has changed
    emit patchingChanged();
}
void UpdateHub::changeDroidState()
{
    // Called when the state if the Droid SD card or X7 connection has changed.
    emit droidStateChanged();
}
