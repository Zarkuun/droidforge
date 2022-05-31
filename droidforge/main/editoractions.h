#ifndef EDITORACTIONS_H
#define EDITORACTIONS_H

#include <QObject>
#include <QAction>

typedef enum {
    ACTION_ABORT_PATCHING,
    ACTION_ADD_CONTROLLER,
    ACTION_ADD_JACK,
    ACTION_COPY,
    ACTION_CUT,
    ACTION_DELETE_PATCH_SECTION,
    ACTION_DUPLICATE_PATCH_SECTION,
    ACTION_EDIT_CIRCUIT_COMMENT,
    ACTION_EDIT_VALUE,
    ACTION_EXPORT_SELECTION,
    ACTION_FINISH_PATCHING,
    ACTION_FOLLOW_INTERNAL_CABLE,
    ACTION_INTEGRATE_PATCH,
    ACTION_JUMP_TO_NEXT_PROBLEM,
    ACTION_MERGE_WITH_LEFT_SECTION,
    ACTION_MERGE_WITH_RIGHT_SECTION,
    ACTION_MOVE_INTO_SECTION,
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
    ACTION_RENAME_INTERNAL_CABLE,
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

class EditorActions : public QObject
{
    Q_OBJECT
    QAction *actions[NUM_ACTIONS];

public:
    explicit EditorActions(QObject *parent = nullptr);
    QAction *action(action_t action) { return actions[action]; };

private:
    void createActions();
    QIcon icon(QString what) const; // TODO: Move to new IconBuilder class
};

extern EditorActions *the_actions;

/* Rule: for working with the actions:
 * - The connect() is always be done by the class that receives
 *   the action signal, not by someone else.
*/
#define CONNECT_ACTION(A, FUNC)  connect(the_actions->action(A), &QAction::triggered, this, FUNC)
#define ADD_ACTION(A, TO)        TO->addAction(the_actions->action(A))

#endif // EDITORACTIONS_H
