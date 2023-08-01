#ifndef EDITORACTIONS_H
#define EDITORACTIONS_H

#include "os.h"
#include "selection.h"
#include "patcheditengine.h"
#include "patchview.h"

#include <QObject>
#include <QAction>

typedef enum {
    ACTION_ABORT_ALL_ACTIONS,
    ACTION_ABORT_PATCHING,
    ACTION_ABOUT,
    ACTION_ADD_MISSING_JACKS,
    ACTION_BARE_PATCH_SOURCE,
    ACTION_CIRCUIT_MANUAL,
    ACTION_CLEAR_RECENT_FILES,
    ACTION_CLEAR_SETTINGS,
    ACTION_CLOSE_WINDOW,
    ACTION_CONFIGURE_COLORS,
    ACTION_COPY,
    ACTION_CUT,
    ACTION_DELETE_PATCH_SECTION,
    ACTION_DISABLE,
    ACTION_DISCORD,
    ACTION_DUPLICATE_CIRCUIT,
    ACTION_DUPLICATE_PATCH_SECTION,
    ACTION_DUPLICATE_PATCH_SECTION_SMARTLY,
    ACTION_EDIT_SECTION_COMMENT,
    ACTION_EDIT_CIRCUIT_COMMENT,
    ACTION_EDIT_CIRCUIT_SOURCE,
    ACTION_EDIT_JACK_COMMENT,
    ACTION_EDIT_LABEL,
    ACTION_EDIT_PATCH_SOURCE,
    ACTION_EDIT_SECTION_SOURCE,
    ACTION_EDIT_VALUE,
    ACTION_ENABLE,
    ACTION_EXPAND_ARRAY,
    ACTION_EXPAND_ARRAY_MAX,
    ACTION_EXPORT_SELECTION,
    ACTION_FIND,
    ACTION_FINISH_PATCHING,
    ACTION_FIX_LED_MISMATCH,
    ACTION_FOLD_UNFOLD,
    ACTION_FOLD_UNFOLD_ALL,
    ACTION_FOLLOW_CABLE,
    ACTION_FOLLOW_REGISTER,
    ACTION_INTEGRATE_PATCH,
    ACTION_JUMP_TO_BOOKMARK,
    ACTION_JUMP_TO_NEXT_PROBLEM,
    ACTION_MIRROR_PLUGS,
    ACTION_LICENSE,
    ACTION_MERGE_ALL_SECTIONS,
    ACTION_MERGE_WITH_NEXT_SECTION,
    ACTION_MERGE_WITH_PREVIOUS_SECTION,
    ACTION_MOVE_CIRCUIT_DOWN,
    ACTION_MOVE_CIRCUIT_UP,
    ACTION_MOVE_SECTION_DOWN,
    ACTION_MOVE_SECTION_UP,
    ACTION_NEW,
    ACTION_NEW_CIRCUIT,
    ACTION_NEW_CONTROLLER,
    ACTION_NEW_JACK,
    ACTION_NEW_PATCH_SECTION,
    ACTION_NEW_WINDOW,
    ACTION_NEW_WINDOW_WITH_SAME_RACK,
    ACTION_NEW_WITH_SAME_RACK,
    ACTION_NEXT_SECTION,
    ACTION_OPEN,
    ACTION_OPEN_ENCLOSING_FOLDER,
    ACTION_OPEN_IN_NEW_WINDOW,
    ACTION_PASTE,
    ACTION_PASTE_AS_SECTION,
    ACTION_PASTE_SMARTLY,
    ACTION_PATCH_PROPERTIES,
    ACTION_PREFERENCES,
    ACTION_PREVIOUS_SECTION,
    ACTION_QUIT,
    ACTION_RACK_RESET_ZOOM,
    ACTION_RACK_ZOOM_IN,
    ACTION_RACK_ZOOM_OUT,
    ACTION_REDO,
    ACTION_REMOVE_UNDEFINED_JACKS,
    ACTION_RENAME_CABLE,
    ACTION_RENAME_PATCH_SECTION,
    ACTION_RESET_ZOOM,
    ACTION_REWRITE_CABLE_NAMES,
    ACTION_RIGHT_TO_LEFT,
    ACTION_SAVE,
    ACTION_SAVE_AS,
    ACTION_SAVE_ALL,
    ACTION_SAVE_SECTION,
    ACTION_SAVE_TO_SD,
    ACTION_SELECT_ALL,
    ACTION_SET_BOOKMARK,
    ACTION_SHOW_ONE_G8,
    ACTION_SHOW_TWO_G8,
    ACTION_SHOW_THREE_G8,
    ACTION_SHOW_FOUR_G8,
    ACTION_SHOW_USED_G8s,
    ACTION_SHOW_REGISTER_LABELS,
    ACTION_SHOW_REGISTER_USAGE,
    ACTION_SHOW_X7_ON_DEMAND,
    ACTION_SORT_JACKS,
    ACTION_START_PATCHING,
    ACTION_TEXT_MODE,
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
    ACTION_USER_MANUAL,
    ACTION_ZOOM_IN,
    ACTION_ZOOM_OUT,
    ACTION_NEXT_WINDOW,
    ACTION_PREVIOUS_WINDOW,
    ACTION_MINIMIZE_WINDOW,
    NUM_ACTIONS,
} action_t;


class MainWindow;
class PatchOperator;

class EditorActions : public QObject, PatchView
{
    Q_OBJECT
    MainWindow *mainWindow;
    QAction *actions[NUM_ACTIONS];

public:
    explicit EditorActions(MainWindow *mainWindow, PatchEditEngine *patch, QObject *parent = nullptr);
    QAction *action(action_t action) { return actions[action]; };
    void updateIcons();

private slots:
    void modifyPatch();
    void switchSection();
    void changeClipboard();
    void changeSelection();
    void moveCursor();
    void changePatching();
    void changeDroidState();

private:
    PatchOperator *theOperator();
    void createActions();
    void updateDisablingActions();
    void updatePasteAction();
    void updateUploadAction(action_t action);
    void updateSaveToSDAction(action_t action);
    void persistViewToggles();
};

/* Rule: for working with the actions:
 * - The connect() is always be done by the class that receives
 *   the action signal, not by someone else.
*/
#define ACTION(A)                      (mainWindow->theActions()->action(A))
#define CONNECT_ACTION(A, FUNC)        connect(ACTION(A), &QAction::triggered, this, FUNC)
#define SET_ACTION_TRIGGER(A, FUNC)    connect(this, FUNC, ACTION(A), &QAction::trigger)
#define TRIGGER_ACTION(A)              ACTION(A)->trigger();
#define ADD_ACTION(A, TO)              TO->addAction(ACTION(A))
#define ADD_ACTION_IF_ENABLED(A, TO)   if (ACTION(A)->isEnabled()) TO->addAction(ACTION(A))

#endif // EDITORACTIONS_H
