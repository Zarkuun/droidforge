#include "patchoperator.h"
#include "updatehub.h"

PatchOperator *the_operator = 0;

PatchOperator::PatchOperator(PatchEditEngine *patch)
    : patch(patch)
{
    Q_ASSERT(the_operator == 0);
    the_operator = this;

    CONNECT_ACTION(ACTION_UNDO, &PatchOperator::undo);
    CONNECT_ACTION(ACTION_REDO, &PatchOperator::redo);

    // Events that we create
    connect(this, &PatchOperator::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchOperator::clipboardChanged, the_hub, &UpdateHub::changeClipboard);
    connect(this, &PatchOperator::selectionChanged, the_hub, &UpdateHub::changeSelection);
    connect(this, &PatchOperator::sectionSwitched, the_hub, &UpdateHub::switchSection);
    connect(this, &PatchOperator::cursorMoved, the_hub, &UpdateHub::moveCursor);
    connect(this, &PatchOperator::patchingChanged, the_hub, &UpdateHub::changePatching);
}

void PatchOperator::undo()
{
    qDebug("ICH UNDOE");
    if (patch->undoPossible()) {
        patch->undo();
        emit patchModified();
    }
}

void PatchOperator::redo()
{
    if (patch->redoPossible()) {
        patch->redo();
        emit patchModified();
    }
}
