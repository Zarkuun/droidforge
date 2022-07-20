#ifndef EDITORACTIONS_H
#define EDITORACTIONS_H

#include "selection.h"
#include "patcheditengine.h"
#include "patchview.h"

#include <QObject>
#include <QAction>

typedef enum {
    ACTION_CLEAR_RECENT_FILES,
    ACTION_CLEAR_SETTINGS,
    ACTION_MOVE_SECTION_UP,
    ACTION_MOVE_SECTION_DOWN,
    ACTION_LICENSE,
    ACTION_ABOUT,
    ACTION_USER_MANUAL,
    ACTION_CIRCUIT_MANUAL,
    ACTION_FIX_LED_MISMATCH,
    ACTION_ABORT_ALL_ACTIONS,
    ACTION_EDIT_CIRCUIT_SOURCE,
    ACTION_EDIT_SECTION_SOURCE,
    ACTION_EDIT_PATCH_SOURCE,
    ACTION_BARE_PATCH_SOURCE,
    ACTION_ABORT_PATCHING,
    ACTION_ADD_CONTROLLER,
    ACTION_ADD_JACK,
    ACTION_ADD_MISSING_JACKS,
    ACTION_CONFIGURE_COLORS,
    ACTION_COPY,
    ACTION_CUT,
    ACTION_DELETE_PATCH_SECTION,
    ACTION_DISABLE,
    ACTION_DUPLICATE_PATCH_SECTION,
    ACTION_EDIT_CIRCUIT_COMMENT,
    ACTION_EDIT_JACK_COMMENT,
    ACTION_EDIT_VALUE,
    ACTION_ENABLE,
    ACTION_EXPAND_ARRAY,
    ACTION_EXPAND_ARRAY_MAX,
    ACTION_EXPORT_SELECTION,
    ACTION_FINISH_PATCHING,
    ACTION_FOLD_UNFOLD,
    ACTION_FOLD_UNFOLD_ALL,
    ACTION_FOLLOW_CABLE,
    ACTION_INTEGRATE_PATCH,
    ACTION_JUMP_TO_NEXT_PROBLEM,
    ACTION_MERGE_ALL_SECTIONS,
    ACTION_MERGE_WITH_NEXT_SECTION,
    ACTION_MERGE_WITH_PREVIOUS_SECTION,
    ACTION_MOVE_CIRCUIT_DOWN,
    ACTION_MOVE_CIRCUIT_UP,
    ACTION_NEW,
    ACTION_NEW_CIRCUIT,
    ACTION_NEW_PATCH_SECTION,
    ACTION_NEXT_SECTION,
    ACTION_OPEN,
    ACTION_OPEN_ENCLOSING_FOLDER,
    ACTION_PASTE,
    ACTION_PASTE_AS_SECTION,
    ACTION_PASTE_SMART,
    ACTION_PATCH_PROPERTIES,
    ACTION_PREVIOUS_SECTION,
    ACTION_QUIT,
    ACTION_REDO,
    ACTION_REMOVE_UNDEFINED_JACKS,
    ACTION_RENAME_CABLE,
    ACTION_RENAME_PATCH_SECTION,
    ACTION_RESET_ZOOM,
    ACTION_RACK_RESET_ZOOM,
    ACTION_SAVE,
    ACTION_SAVE_AS,
    ACTION_SAVE_TO_SD,
    ACTION_SELECT_ALL,
    ACTION_SORT_JACKS,
    ACTION_START_PATCHING,
    ACTION_TOOLBAR_ADD_CONTROLLER,
    ACTION_TOOLBAR_ADD_JACK,
    ACTION_TOOLBAR_NEW,
    ACTION_TOOLBAR_NEW_CIRCUIT,
    ACTION_TOOLBAR_OPEN,
    ACTION_TOOLBAR_PROBLEMS,
    ACTION_TOOLBAR_SAVE,
    ACTION_TOOLBAR_SAVE_TO_SD,
    ACTION_TOOLBAR_UPLOAD_TO_DROID,
    ACTION_UNDO,
    ACTION_UPLOAD_TO_DROID,
    ACTION_RACK_ZOOM_IN,
    ACTION_RACK_ZOOM_OUT,
    ACTION_ZOOM_IN,
    ACTION_ZOOM_OUT,

    NUM_ACTIONS,
} action_t;

class EditorActions : public QObject, PatchView
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
    void changeSelection();
    void moveCursor();
    void changePatching();
    void changeDroidState();

private:
    void createActions();
    void updateDisablingActions();
    void updatePasteAction();
};

extern EditorActions *the_actions;

/* Rule: for working with the actions:
 * - The connect() is always be done by the class that receives
 *   the action signal, not by someone else.
*/
#define ACTION(A)                      (the_actions->action(A))
#define CONNECT_ACTION(A, FUNC)        connect(the_actions->action(A), &QAction::triggered, this, FUNC)
#define SET_ACTION_TRIGGER(A, FUNC)    connect(this, FUNC, the_actions->action(A), &QAction::trigger)
#define TRIGGER_ACTION(A)              the_actions->action(A)->trigger();
#define ADD_ACTION(A, TO)              TO->addAction(the_actions->action(A))
#define ADD_ACTION_IF_ENABLED(A, TO)   if (the_actions->action(A)->isEnabled()) TO->addAction(the_actions->action(A))

#endif // EDITORACTIONS_H
