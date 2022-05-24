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
    QString getName() const { return name; };
};

#endif // EDITORSTATE_H
