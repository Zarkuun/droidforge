#ifndef EDITORACTIONS_H
#define EDITORACTIONS_H

#include "selection.h"
#include "patcheditengine.h"
#include "patchoperator.h"

#include <QObject>
#include <QAction>

typedef enum {
    ACTION_ABORT_PATCHING,
    ACTION_ADD_CONTROLLER,
    ACTION_ADD_JACK,
    ACTION_COPY,
    ACTION_CUT,
    ACTION_DELETE_PATCH_SECTION,
    ACTION_DISABLE,
    ACTION_ENABLE,
    ACTION_DUPLICATE_PATCH_SECTION,
    ACTION_EDIT_CIRCUIT_COMMENT,
    ACTION_EDIT_JACK_COMMENT,
    ACTION_EDIT_VALUE,
    ACTION_EXPORT_SELECTION,
    ACTION_FINISH_PATCHING,
    ACTION_FOLLOW_CABLE,
    ACTION_INTEGRATE_PATCH,
    ACTION_JUMP_TO_NEXT_PROBLEM,
    ACTION_MERGE_WITH_PREVIOUS_SECTION,
    ACTION_MERGE_WITH_NEXT_SECTION,
    ACTION_MERGE_ALL_SECTIONS,
    ACTION_CREATE_SECTION_FROM_SELECTION,
    ACTION_NEW,
    ACTION_NEW_CIRCUIT,
    ACTION_NEW_PATCH_SECTION,
    ACTION_NEXT_SECTION,
    ACTION_OPEN,
    ACTION_OPEN_ENCLOSING_FOLDER,
    ACTION_PATCH_PROPERTIES,
    ACTION_PASTE,
    ACTION_PASTE_SMART,
    ACTION_PREVIOUS_SECTION,
    ACTION_QUIT,
    ACTION_REDO,
    ACTION_RENAME_CABLE,
    ACTION_RENAME_PATCH_SECTION,
    ACTION_RESET_ZOOM,
    ACTION_SAVE,
    ACTION_SAVE_AS,
    ACTION_START_PATCHING,
    ACTION_UNDO,
    ACTION_ZOOM_IN,
    ACTION_ZOOM_OUT,

    NUM_ACTIONS,
} action_t;

class EditorActions : public QObject, PatchOperator
{
    Q_OBJECT
    QAction *actions[NUM_ACTIONS];

public:
    explicit EditorActions(PatchEditEngine *patch, QObject *parent = nullptr);
    QAction *action(action_t action) { return actions[action]; };

private slots:
    void modifyPatch();
    void switchSection();
    void changeClipboard();
    void changeSelection(const Selection *selection);
    void moveCursor();
    void changePatching();

private:
    void createActions();
    QIcon icon(QString what) const; // TODO: Move to new IconBuilder class
    void updateDisablingActions();
};

extern EditorActions *the_actions;

/* Rule: for working with the actions:
 * - The connect() is always be done by the class that receives
 *   the action signal, not by someone else.
*/
#define CONNECT_ACTION(A, FUNC)      connect(the_actions->action(A), &QAction::triggered, this, FUNC)
#define TRIGGER_ACTION(A, FUNC)      connect(this, FUNC, the_actions->action(A), &QAction::trigger)
#define ADD_ACTION(A, TO)            TO->addAction(the_actions->action(A))
#define ADD_ACTION_IF_ENABLED(A, TO) if (the_actions->action(A)->isEnabled()) TO->addAction(the_actions->action(A))


#endif // EDITORACTIONS_H
