#include "editoractions.h"
#include "iconbase.h"
#include "globals.h"
#include "patchoperator.h"
#include "updatehub.h"
#include "clipboard.h"

EditorActions *the_actions = 0;

EditorActions::EditorActions(PatchEditEngine *patch, QObject *parent)
    : QObject{parent}
    , PatchView(patch)
{
    Q_ASSERT(the_actions == 0);
    the_actions = this;
    createActions();

    // Events that we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &EditorActions::switchSection);
    connect(the_hub, &UpdateHub::patchModified, this, &EditorActions::modifyPatch);
    connect(the_hub, &UpdateHub::clipboardChanged, this, &EditorActions::changeClipboard);
    connect(the_hub, &UpdateHub::selectionChanged, this, &EditorActions::changeSelection);
    connect(the_hub, &UpdateHub::cursorMoved, this, &EditorActions::moveCursor);
    connect(the_hub, &UpdateHub::patchingChanged, this, &EditorActions::changePatching);
    connect(the_hub, &UpdateHub::droidStateChanged, this, &EditorActions::changeDroidState);
}

void EditorActions::createActions()
{
    actions[ACTION_ABOUT] = new QAction(tr("About DROID Forge"), this);
    actions[ACTION_LICENSE] = new QAction(tr("DROID Forge license"), this);

    actions[ACTION_CLEAR_SETTINGS] = new QAction(tr("Restore factory settings"), this);

    actions[ACTION_CLEAR_RECENT_FILES] = new QAction(tr("Clear list of recent files"), this);

    actions[ACTION_NEW] = new QAction(tr("&New..."), this);
    actions[ACTION_NEW]->setShortcut(QKeySequence(tr("Ctrl+Shift+Alt+N")));
    actions[ACTION_TOOLBAR_NEW] = new QAction(ICON("new"), tr("New"), this);

    actions[ACTION_CONFIGURE_COLORS] = new QAction(tr("Edit colors"), this);
    actions[ACTION_CONFIGURE_COLORS]->setShortcut(QKeySequence(tr("F7")));

    actions[ACTION_OPEN] = new QAction(tr("&Open..."), this);
    actions[ACTION_OPEN]->setShortcuts(QKeySequence::Open);
    actions[ACTION_TOOLBAR_OPEN] = new QAction(ICON("open"), tr("Open"), this);

    actions[ACTION_SAVE] = new QAction(tr("&Save..."), this);
    actions[ACTION_SAVE]->setShortcuts(QKeySequence::Save);
    actions[ACTION_TOOLBAR_SAVE] = new QAction(ICON("save"), tr("Save"), this);

    actions[ACTION_SAVE_AS] = new QAction(tr("Save &as..."), this);
    actions[ACTION_SAVE_AS]->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));

    actions[ACTION_EXPORT_SELECTION] = new QAction(tr("E&xport selection as patch..."), this);
    actions[ACTION_EXPORT_SELECTION]->setEnabled(false);

    actions[ACTION_UPLOAD_TO_DROID] = new QAction(tr("Activate in master via USB to X7"), this);
    actions[ACTION_UPLOAD_TO_DROID]->setShortcut(QKeySequence(tr("F9")));
    actions[ACTION_TOOLBAR_UPLOAD_TO_DROID] = new QAction(ICON("upload_to_droid"), tr("Activate!"), this);

    actions[ACTION_SAVE_TO_SD] = new QAction(tr("Save to DROID microSD card"), this);
    actions[ACTION_SAVE_TO_SD]->setShortcut(QKeySequence(tr("F10")));
    actions[ACTION_TOOLBAR_SAVE_TO_SD] = new QAction(ICON("save_to_sd"), tr("Save to SD"), this);

    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    #ifdef Q_OS_MACOS
    QString title = tr("Reveal in finder");
    #else
    QString title = tr("Re&veal in explorer");
    #endif
    actions[ACTION_OPEN_ENCLOSING_FOLDER] = new QAction(title, this);
    #endif
    actions[ACTION_INTEGRATE_PATCH] = new QAction(tr("&Integrate other patch"), this);
    actions[ACTION_INTEGRATE_PATCH]->setShortcut(QKeySequence(tr("Ctrl+I")));

    actions[ACTION_JUMP_TO_NEXT_PROBLEM] = new QAction(tr("&Jump to next problem"), this);
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setShortcut(QKeySequence(tr("F6")));

    actions[ACTION_TOOLBAR_PROBLEMS] = new QAction(ICON("problems"), tr("Problems"), this);

    actions[ACTION_QUIT] = new QAction(tr("&Quit"), this);
    actions[ACTION_QUIT]->setShortcuts(QKeySequence::Quit);

    actions[ACTION_PATCH_PROPERTIES] = new QAction(tr("&Patch properties..."), this);
    actions[ACTION_PATCH_PROPERTIES]->setShortcut(QKeySequence(tr("Ctrl+.")));

    actions[ACTION_UNDO] = new QAction(tr("&Undo"), this);
    actions[ACTION_UNDO]->setShortcuts(QKeySequence::Undo);

    actions[ACTION_REDO] = new QAction(tr("&Redo"), this);
    actions[ACTION_REDO]->setShortcuts(QKeySequence::Redo);

    actions[ACTION_CUT] = new QAction(tr("C&ut"), this);
    actions[ACTION_CUT]->setShortcuts(QKeySequence::Cut);

    actions[ACTION_COPY] = new QAction(tr("&Copy"), this);
    actions[ACTION_COPY]->setShortcuts(QKeySequence::Copy);

    actions[ACTION_PASTE] = new QAction(tr("&Paste"), this);
    actions[ACTION_PASTE]->setShortcuts(QKeySequence::Paste);
    actions[ACTION_PASTE]->setEnabled(false); // enabled by clipboard

    actions[ACTION_PASTE_SMART] = new QAction(tr("&Paste smart"), this);
    actions[ACTION_PASTE_SMART]->setShortcut(QKeySequence(tr("Shift+Ctrl+V")));
    actions[ACTION_PASTE_SMART]->setEnabled(false); // enabled by clipboard

    actions[ACTION_PASTE_AS_SECTION] = new QAction(tr("Paste as new section"), this);
    actions[ACTION_PASTE_AS_SECTION]->setShortcut(QKeySequence("Meta+V"));
    actions[ACTION_PASTE_AS_SECTION]->setEnabled(false);

    actions[ACTION_EXPAND_ARRAY] = new QAction(tr("Expand parameter array by one"), this);
    actions[ACTION_EXPAND_ARRAY]->setShortcut(QKeySequence(tr("Ctrl+E")));

    actions[ACTION_EXPAND_ARRAY_MAX] = new QAction(tr("Expand parameter array to max"), this);
    actions[ACTION_EXPAND_ARRAY_MAX]->setShortcut(QKeySequence(tr("Shift+Ctrl+E")));

    actions[ACTION_ADD_MISSING_JACKS] = new QAction(tr("Add remaining parameters"), this);
    actions[ACTION_ADD_MISSING_JACKS]->setShortcut(QKeySequence(tr("Ctrl+R")));

    actions[ACTION_REMOVE_UNDEFINED_JACKS] = new QAction(tr("Remove undefined parameters"), this);
    actions[ACTION_REMOVE_UNDEFINED_JACKS]->setShortcut(QKeySequence(tr("Shift+Ctrl+R")));

    actions[ACTION_FIX_LED_MISMATCH] = new QAction(tr("Fix button/LED mismatches"), this);
    actions[ACTION_FIX_LED_MISMATCH]->setShortcut(QKeySequence(tr("Ctrl+L")));

    actions[ACTION_SELECT_ALL] = new QAction(tr("Select all"), this);
    actions[ACTION_SELECT_ALL]->setShortcut(QKeySequence(tr("Ctrl+A")));

    actions[ACTION_MOVE_CIRCUIT_UP] = new QAction(tr("Move circuit up"), this);
    actions[ACTION_MOVE_CIRCUIT_UP]->setShortcut(QKeySequence(tr("Shift+Ctrl+Up")));

    actions[ACTION_MOVE_CIRCUIT_DOWN] = new QAction(tr("Move circuit down"), this);
    actions[ACTION_MOVE_CIRCUIT_DOWN]->setShortcut(QKeySequence(tr("Shift+Ctrl+Down")));

    actions[ACTION_DISABLE] = new QAction(tr("&Disable"), this);
    actions[ACTION_DISABLE]->setShortcut(QKeySequence(tr("#")));

    actions[ACTION_ENABLE] = new QAction(tr("&Enable"), this);
    actions[ACTION_ENABLE]->setShortcut(QKeySequence(tr("#")));

    actions[ACTION_SORT_JACKS] = new QAction(tr("Sort parameters"), this);
    actions[ACTION_SORT_JACKS]->setShortcut(QKeySequence(tr("Meta+S")));

    actions[ACTION_NEW_CIRCUIT] = new QAction(tr("&New circuit..."), this);
    actions[ACTION_NEW_CIRCUIT]->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));
    actions[ACTION_TOOLBAR_NEW_CIRCUIT] = new QAction(ICON("new_circuit"), tr("Circuit..."), this);

    actions[ACTION_NEW_JACK] = new QAction(tr("&New parameter..."), this);
    actions[ACTION_NEW_JACK]->setShortcut(QKeySequence(tr("Ctrl+N")));
    actions[ACTION_TOOLBAR_ADD_JACK] = new QAction(ICON("add_jack"), tr("Parameter"), this);

    actions[ACTION_EDIT_VALUE] = new QAction(tr("&Edit element under cursor..."), this);
    actions[ACTION_EDIT_VALUE]->setShortcuts({ QKeySequence(tr("Enter")),
                                    QKeySequence(tr("Return"))});
    actions[ACTION_FOLLOW_CABLE] = new QAction(tr("&Follow internal cable"), this);
    actions[ACTION_FOLLOW_CABLE]->setShortcut(QKeySequence(tr("?")));

    actions[ACTION_RENAME_CABLE] = new QAction(tr("&Rename internal cable"), this);
    actions[ACTION_RENAME_CABLE]->setShortcut(QKeySequence(tr("Alt+R")));

    actions[ACTION_START_PATCHING] = new QAction(tr("Start creating internal cable"), this);
    actions[ACTION_START_PATCHING]->setShortcut(QKeySequence(tr("=")));

    actions[ACTION_FINISH_PATCHING] = new QAction(tr("Finish creating internal cable"), this);
    actions[ACTION_FINISH_PATCHING]->setShortcut(QKeySequence(tr("=")));
    actions[ACTION_FINISH_PATCHING]->setEnabled(false);

    actions[ACTION_ABORT_PATCHING] = new QAction(tr("Abort creating internal cable"), this);
    actions[ACTION_ABORT_PATCHING]->setEnabled(false);

    actions[ACTION_ABORT_ALL_ACTIONS] = new QAction(tr("Abort all actions"), this);
    actions[ACTION_ABORT_ALL_ACTIONS]->setShortcut(QKeySequence(tr("Escape")));

    actions[ACTION_EDIT_CIRCUIT_COMMENT] = new QAction(tr("Edit circuit comment..."), this);
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));

    actions[ACTION_EDIT_JACK_COMMENT] = new QAction(tr("Edit parameter info..."), this);
    actions[ACTION_EDIT_JACK_COMMENT]->setShortcut(QKeySequence(tr("Alt+C")));

    actions[ACTION_PREVIOUS_SECTION] = new QAction(tr("Go to previous section"));
    actions[ACTION_PREVIOUS_SECTION]->setShortcut(QKeySequence(tr("Ctrl+Up")));

    actions[ACTION_NEXT_SECTION] = new QAction(tr("Go to next section"));
    actions[ACTION_NEXT_SECTION]->setShortcut(QKeySequence(tr("Ctrl+Down")));

    actions[ACTION_MOVE_SECTION_UP] = new QAction(tr("Move by one position up"));
    actions[ACTION_MOVE_SECTION_UP]->setShortcut(QKeySequence(tr("Meta+Shift+Up")));

    actions[ACTION_MOVE_SECTION_DOWN] = new QAction(tr("Move by one position down"));
    actions[ACTION_MOVE_SECTION_DOWN]->setShortcut(QKeySequence(tr("Meta+Shift+Down")));

    actions[ACTION_NEW_PATCH_SECTION] = new QAction(tr("New section..."), this);
    actions[ACTION_NEW_PATCH_SECTION]->setShortcut(QKeySequence(tr("Meta+N")));

    actions[ACTION_DUPLICATE_PATCH_SECTION] = new QAction(tr("Duplicate section..."), this);
    actions[ACTION_DUPLICATE_PATCH_SECTION]->setShortcut(QKeySequence(tr("Meta+D")));

    actions[ACTION_RENAME_PATCH_SECTION] = new QAction(tr("Rename section..."), this);
    actions[ACTION_RENAME_PATCH_SECTION]->setShortcut(QKeySequence(tr("Meta+R")));

    actions[ACTION_DELETE_PATCH_SECTION] = new QAction(tr("Delete section"), this);
    actions[ACTION_DELETE_PATCH_SECTION]->setShortcut(QKeySequence(tr("Meta+Backspace")));

    actions[ACTION_MERGE_WITH_PREVIOUS_SECTION] = new QAction(tr("Merge with previous section"));

    actions[ACTION_MERGE_WITH_NEXT_SECTION] = new QAction(tr("Merge with next section"));
    actions[ACTION_MERGE_ALL_SECTIONS] = new QAction(tr("Merge all sections into one"));

    actions[ACTION_RESET_ZOOM] = new QAction(tr("Normal font size"), this);
    actions[ACTION_RESET_ZOOM]->setShortcut(QKeySequence(tr("Ctrl+0")));

    actions[ACTION_ZOOM_IN] = new QAction(tr("Increase font size"), this);
    actions[ACTION_ZOOM_IN]->setShortcuts(QKeySequence::ZoomIn);

    actions[ACTION_ZOOM_OUT] = new QAction(tr("Decrease font size"), this);
    actions[ACTION_ZOOM_OUT]->setShortcuts(QKeySequence::ZoomOut);

    actions[ACTION_RACK_RESET_ZOOM] = new QAction(tr("Normal rack size"), this);
    actions[ACTION_RACK_RESET_ZOOM]->setShortcut(QKeySequence(tr("Alt+Ctrl+0")));

    actions[ACTION_RACK_ZOOM_IN] = new QAction(tr("Increase rack size"), this);
    actions[ACTION_RACK_ZOOM_IN]->setShortcut(QKeySequence(tr("Alt+Ctrl+=")));

    actions[ACTION_RACK_ZOOM_OUT] = new QAction(tr("Decrease rack size"), this);
    actions[ACTION_RACK_ZOOM_OUT]->setShortcut(QKeySequence(tr("Alt+Ctrl+-")));

    actions[ACTION_FOLD_UNFOLD] = new QAction(tr("Fold / unfold circuit"), this);
    actions[ACTION_FOLD_UNFOLD]->setShortcut(QKeySequence(tr("Space")));

    actions[ACTION_FOLD_UNFOLD_ALL] = new QAction(tr("Fold / unfold all circuits"), this);
    actions[ACTION_FOLD_UNFOLD_ALL]->setShortcut(QKeySequence(tr("Shift+Space")));

    actions[ACTION_SHOW_REGISTER_LABELS] = new QAction(tr("Show register labels"), this);
    actions[ACTION_SHOW_REGISTER_LABELS]->setShortcut(QKeySequence(tr("F3")));
    actions[ACTION_SHOW_REGISTER_LABELS]->setCheckable(true);
    actions[ACTION_SHOW_REGISTER_LABELS]->setChecked(true);

    actions[ACTION_SHOW_REGISTER_USAGE] = new QAction(tr("Show used registers"), this);
    actions[ACTION_SHOW_REGISTER_USAGE]->setShortcut(QKeySequence(tr("F4")));
    actions[ACTION_SHOW_REGISTER_USAGE]->setCheckable(true);
    actions[ACTION_SHOW_REGISTER_USAGE]->setChecked(true);

    actions[ACTION_NEW_CONTROLLER] = new QAction(tr("New controller..."), this);
    actions[ACTION_NEW_CONTROLLER]->setShortcut(QKeySequence(tr("Ctrl+Alt+N")));
    actions[ACTION_TOOLBAR_ADD_CONTROLLER] = new QAction(ICON("add_controller"), tr("Controller"), this);

    actions[ACTION_EDIT_CIRCUIT_SOURCE] = new QAction(tr("Edit circuit source code"), this);
    actions[ACTION_EDIT_CIRCUIT_SOURCE]->setShortcut(QKeySequence(tr("Ctrl+U")));

    actions[ACTION_EDIT_SECTION_SOURCE] = new QAction(tr("Edit section source code"), this);
    actions[ACTION_EDIT_SECTION_SOURCE]->setShortcut(QKeySequence(tr("Meta+U")));

    actions[ACTION_EDIT_PATCH_SOURCE] = new QAction(tr("Edit patch source code"), this);
    actions[ACTION_EDIT_PATCH_SOURCE]->setShortcut(QKeySequence(tr("Shift+Ctrl+U")));

    actions[ACTION_BARE_PATCH_SOURCE] = new QAction(tr("Show bare patch source"), this);
    actions[ACTION_BARE_PATCH_SOURCE]->setShortcut(QKeySequence(tr("Alt+Ctrl+U")));

    actions[ACTION_USER_MANUAL] = new QAction(tr("User manual"), this);
    actions[ACTION_USER_MANUAL]->setShortcuts({QKeySequence(tr("Shift+Ctrl+M")), QKeySequence(tr("F1"))});

    actions[ACTION_CIRCUIT_MANUAL] = new QAction(tr("User manual of circuit"), this);
    actions[ACTION_CIRCUIT_MANUAL]->setShortcut(QKeySequence(tr("Ctrl+M")));

    for (auto action: actions)
         action->setShortcutVisibleInContextMenu(true);
}
void EditorActions::modifyPatch()
{
    bool maySave = patch->isModified() || !patch->hasFilename();
    actions[ACTION_SAVE]->setEnabled(maySave);
    actions[ACTION_TOOLBAR_SAVE]->setEnabled(maySave);
    actions[ACTION_OPEN_ENCLOSING_FOLDER]->setEnabled(patch->getFilePath() != "");

    if (patch->undoPossible()) {
        actions[ACTION_UNDO]->setText(tr("&Undo ") + patch->nextUndoTitle());
        actions[ACTION_UNDO]->setEnabled(true);
    }
    else {
        actions[ACTION_UNDO]->setText(tr("&Undo"));
        actions[ACTION_UNDO]->setEnabled(false);
    }

    if (patch->redoPossible()) {
        actions[ACTION_REDO]->setText(tr("&Redo ") + patch->nextRedoTitle());
        actions[ACTION_REDO]->setEnabled(true);
    }
    else {
        actions[ACTION_REDO]->setText(tr("&Redo"));
        actions[ACTION_REDO]->setEnabled(false);
    }

    actions[ACTION_DELETE_PATCH_SECTION]->setEnabled(patch->numSections() >= 2);
    actions[ACTION_MERGE_ALL_SECTIONS]->setEnabled(patch->numSections() >= 2);

    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setEnabled(patch->numProblems() > 0);
    actions[ACTION_TOOLBAR_PROBLEMS]->setVisible(patch->numProblems() > 0);

    switchSection();
    changeDroidState(); // actions for upload and save to SD
}
void EditorActions::switchSection()
{
    bool hasCircuit = !section()->isEmpty();
    int sectionIndex = patch->currentSectionIndex();
    int numSections = patch->numSections();
    bool notAtFirstSection = sectionIndex > 0;
    bool notAtLastSection = sectionIndex + 1 < numSections;

    actions[ACTION_MERGE_WITH_PREVIOUS_SECTION]->setEnabled(notAtFirstSection);
    actions[ACTION_MERGE_WITH_NEXT_SECTION]->setEnabled(notAtLastSection);
    actions[ACTION_NEW_JACK]->setEnabled(hasCircuit);
    actions[ACTION_TOOLBAR_ADD_JACK]->setEnabled(hasCircuit);
    actions[ACTION_EDIT_CIRCUIT_SOURCE]->setEnabled(hasCircuit);
    actions[ACTION_PREVIOUS_SECTION]->setEnabled(notAtFirstSection);
    actions[ACTION_NEXT_SECTION]->setEnabled(notAtLastSection);
    actions[ACTION_MOVE_SECTION_UP]->setEnabled(notAtFirstSection);
    actions[ACTION_MOVE_SECTION_DOWN]->setEnabled(notAtLastSection);
    updatePasteAction();
    moveCursor();
    changeSelection();
}
void EditorActions::changeClipboard()
{
    updatePasteAction();
    actions[ACTION_PASTE_SMART]->setEnabled(the_clipboard->numCircuits());
    actions[ACTION_PASTE_AS_SECTION]->setEnabled(the_clipboard->numCircuits());
}
void EditorActions::changeSelection()
{
    const Selection *selection = section()->getSelection();
    const Circuit *circuit = section()->currentCircuit();
    actions[ACTION_EXPORT_SELECTION]->setEnabled(selection && selection->isCircuitSelection());
    actions[ACTION_CUT]->setEnabled(selection || circuit);
    actions[ACTION_COPY]->setEnabled(selection || circuit);
    updateDisablingActions();
}
void EditorActions::moveCursor()
{
    Circuit *circuit = section()->currentCircuit();

    const Atom *atom = section()->currentAtom();
    actions[ACTION_EDIT_VALUE]->setEnabled(circuit);
    actions[ACTION_FOLLOW_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_RENAME_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setEnabled(circuit);
    actions[ACTION_EDIT_JACK_COMMENT]->setEnabled(section()->currentJackAssignment());
    actions[ACTION_SORT_JACKS]->setEnabled(circuit);

    actions[ACTION_MOVE_CIRCUIT_UP]->setEnabled(circuit && section()->currentCircuitId() > 0);
    actions[ACTION_MOVE_CIRCUIT_DOWN]->setEnabled(circuit && section()->currentCircuitId() + 1 < (int)section()->numCircuits());
    JackAssignment *ja = section()->currentJackAssignment();
    bool expandPossible = circuit && ja && circuit->nextJackArrayName(
                    ja->jackName(), ja->jackType() == JACKTYPE_INPUT) != "";
    actions[ACTION_EXPAND_ARRAY]->setEnabled(expandPossible);
    actions[ACTION_EXPAND_ARRAY_MAX]->setEnabled(expandPossible);
    actions[ACTION_ADD_MISSING_JACKS]->setEnabled(circuit && circuit->hasMissingJacks());
    actions[ACTION_REMOVE_UNDEFINED_JACKS]->setEnabled(circuit && circuit->hasUndefinedJacks());
    actions[ACTION_FIX_LED_MISMATCH]->setEnabled(circuit && circuit->hasLEDMismatch());
    updateDisablingActions();

    actions[ACTION_FOLD_UNFOLD]->setEnabled(circuit);
    actions[ACTION_FOLD_UNFOLD_ALL]->setEnabled(circuit);
    actions[ACTION_START_PATCHING]->setEnabled(section()->cursorPosition().column >= 1 && !patch->isPatching());
    actions[ACTION_CIRCUIT_MANUAL]->setEnabled(circuit);
}
void EditorActions::updateDisablingActions()
{
    const Selection *selection = section()->getSelection();
    bool somethingEnabled = false;
    bool somethingDisabled = false;
    if (selection) {
        const CursorPosition &fromPos = selection->fromPos();
        const CursorPosition &toPos = selection->toPos();
        if (selection->isCircuitSelection())
        {
            for (int i=fromPos.circuitNr; i<=toPos.circuitNr; i++) {
                const Circuit *circuit = section()->circuit(i);
                somethingEnabled |= !circuit->isDisabled();
                somethingDisabled |= circuit->isDisabled();
            }
        }
        else if (selection->isJackSelection())
        {
            const Circuit *circuit = section()->circuit(fromPos.circuitNr);
            for (int i=fromPos.row; i<=toPos.row; i++) {
                if (i < 0)
                    continue;
                const JackAssignment *ja = circuit->jackAssignment(i);
                somethingEnabled |= !ja->isDisabled();
                somethingDisabled |= ja->isDisabled();
            }
        }
    }
    else {
        const JackAssignment *ja = section()->currentJackAssignment();
        if (ja) {
            somethingEnabled = !ja->isDisabled();
            somethingDisabled = ja->isDisabled();
        }
        else {
            const Circuit *circuit = section()->currentCircuit();
            if (circuit) {
                somethingEnabled = !circuit->isDisabled();
                somethingDisabled = circuit->isDisabled();
            }
        }
    }
    // Never enable both, our shortcut '#' would be ambigous otherwise
    actions[ACTION_DISABLE]->setEnabled(somethingEnabled);
    actions[ACTION_ENABLE]->setEnabled(somethingDisabled && !somethingEnabled);

}

void EditorActions::updatePasteAction()
{
    // Cannot paste non-circuits into empty section
    actions[ACTION_PASTE]->setEnabled(!the_clipboard->isEmpty()
      && !(the_clipboard->numCircuits() == 0 && section()->isEmpty()));
}
void EditorActions::changePatching()
{
    const Atom *atom = section()->currentAtom();
    actions[ACTION_START_PATCHING]->setEnabled(atom && !patch->isPatching());
    actions[ACTION_FINISH_PATCHING]->setEnabled(patch->isPatching());
    actions[ACTION_ABORT_PATCHING]->setEnabled(patch->isPatching());
}
void EditorActions::changeDroidState()
{
    updateUploadAction(ACTION_UPLOAD_TO_DROID);
    updateUploadAction(ACTION_TOOLBAR_UPLOAD_TO_DROID);
    updateSaveToSDAction(ACTION_SAVE_TO_SD);
    updateSaveToSDAction(ACTION_TOOLBAR_SAVE_TO_SD);
}
void EditorActions::updateUploadAction(action_t action)
{
    QString tooltip;
    bool enabled = false;
    if (patch->hasProblems())
        tooltip = tr("Your patch has problems. You cannot activate your patch unless you have fixed them.");
    else if (!the_operator->droidX7Present())
        tooltip = tr("No DROID X7 MIDI device was detected. Attach your X7 via USB and push the switch to the right.");
    else {
        tooltip = tr("Upload and activate your patch via MIDI");
        enabled = true;
    }
    actions[action]->setEnabled(enabled);
    actions[action]->setToolTip(tooltip);
}
void EditorActions::updateSaveToSDAction(action_t action)
{
    QString tooltip;
    bool enabled = false;
    if (patch->hasProblems())
        tooltip = tr("Your patch has problems. You cannot activate your patch unless you have fixed them.");
    else if (!the_operator->droidSDCardPresent())
        tooltip = tr("No DROID SD card was detected. Insert your SD card that was already in use by your DROID master.");
    else {
        tooltip = tr("Write your patch to your DROID sd card and eject the card.");
        enabled = true;
    }
    actions[action]->setEnabled(enabled);
    actions[action]->setToolTip(tooltip);
}
