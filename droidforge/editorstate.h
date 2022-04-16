#ifndef EDITORSTATE_H
#define EDITORSTATE_H

#include "patch.h"
#include <QString>

class EditorState
{
    QString name;
    Patch *patch; // lives here

private:
    EditorState(EditorState &);
    EditorState(const EditorState &);

public:
    EditorState(QString name, const Patch *patch);
    ~EditorState();
    Patch *getPatch() const { return patch->clone(); };
};

#endif // EDITORSTATE_H
