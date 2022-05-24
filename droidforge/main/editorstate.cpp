#include "editorstate.h"


EditorState::EditorState(QString name, const Patch *patch)
    : name(name)
    , patch(patch->clone())
{
}


EditorState::~EditorState()
{
    delete patch;
}
