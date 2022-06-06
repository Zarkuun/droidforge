#include "editoractions.h"
#include "iconbase.h"
#include "updatehub.h"
#include "clipboard.h"

EditorActions *the_actions = 0;

QIcon noicon("");

#define NOICON(x) noicon

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
}

void EditorActions::modifyPatch()
{
    actions[ACTION_SAVE]->setEnabled(patch->isModified());
    actions[ACTION_UPLOAD_TO_DROID]->setEnabled(!patch->hasProblems());
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
    actions[ACTION_PROBLEMS]->setVisible(patch->numProblems() > 0);

    switchSection();
}
void EditorActions::switchSection()
{
    int sectionIndex = patch->currentSectionIndex();
    int numSections = patch->numSections();
    actions[ACTION_MERGE_WITH_PREVIOUS_SECTION]->setEnabled(sectionIndex > 0);
    actions[ACTION_MERGE_WITH_NEXT_SECTION]->setEnabled(sectionIndex+1 < numSections);
    moveCursor();
}
void EditorActions::changeClipboard()
{
    actions[ACTION_PASTE]->setEnabled(!the_clipboard->isEmpty());
    actions[ACTION_PASTE_SMART]->setEnabled(the_clipboard->numCircuits());
}
void EditorActions::changeSelection()
{
    const Selection *selection = section()->getSelection();
    actions[ACTION_EXPORT_SELECTION]->setEnabled(selection && selection->isCircuitSelection());
    actions[ACTION_CREATE_SECTION_FROM_SELECTION]->setEnabled(selection && selection->isCircuitSelection());
    updateDisablingActions();
}
void EditorActions::moveCursor()
{
    const Atom *atom = section()->currentAtom();
    actions[ACTION_ADD_JACK]->setEnabled(section()->currentCircuit());
    actions[ACTION_FOLLOW_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_RENAME_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setEnabled(section()->currentCircuit());
    actions[ACTION_EDIT_JACK_COMMENT]->setEnabled(section()->currentJackAssignment());
    actions[ACTION_SORT_JACKS]->setEnabled(section()->currentCircuit());

    Circuit *circuit = section()->currentCircuit();
    JackAssignment *ja = section()->currentJackAssignment();
    bool expandPossible =
                ja && circuit->nextJackArrayName(
                    ja->jackName(), ja->jackType() == JACKTYPE_INPUT) != "";
    actions[ACTION_EXPAND_ARRAY]->setEnabled(expandPossible);
    actions[ACTION_EXPAND_ARRAY_MAX]->setEnabled(expandPossible);
    actions[ACTION_ADD_MISSING_JACKS]->setEnabled(circuit && circuit->hasMissingJacks());
    actions[ACTION_REMOVE_UNDEFINED_JACKS]->setEnabled(circuit && circuit->hasUndefinedJacks());
    updateDisablingActions();

    actions[ACTION_FOLD_UNFOLD]->setEnabled(circuit);
    actions[ACTION_FOLD_UNFOLD_ALL]->setEnabled(circuit);
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
void EditorActions::changePatching()
{
    actions[ACTION_START_PATCHING]->setEnabled(!patch->isPatching());
    actions[ACTION_FINISH_PATCHING]->setEnabled(patch->isPatching());
    actions[ACTION_ABORT_PATCHING]->setEnabled(patch->isPatching());
}
void EditorActions::createActions()
{
    actions[ACTION_NEW] = new QAction(ICON("settings_input_composite"), tr("&New..."), this);
    actions[ACTION_NEW]->setShortcut(QKeySequence(tr("Ctrl+Shift+Alt+N")));
    actions[ACTION_NEW]->setStatusTip(tr("Create a new patch from scratch"));

    actions[ACTION_CONFIGURE_COLORS] = new QAction(tr("Edit colors"), this);
    actions[ACTION_CONFIGURE_COLORS]->setShortcut(QKeySequence(tr("F7")));

    actions[ACTION_OPEN] = new QAction(ICON("open_in_browser"), tr("&Open..."), this);
    actions[ACTION_OPEN]->setShortcuts(QKeySequence::Open);
    actions[ACTION_OPEN]->setStatusTip(tr("Open an existing patch"));

    actions[ACTION_SAVE] = new QAction(ICON("save"), tr("&Save..."), this);
    actions[ACTION_SAVE]->setShortcuts(QKeySequence::Save);
    actions[ACTION_SAVE]->setStatusTip(tr("Save patch to file"));

    actions[ACTION_SAVE_AS] = new QAction(tr("Save &as..."), this);
    actions[ACTION_SAVE_AS]->setShortcuts(QKeySequence::SaveAs);
    actions[ACTION_SAVE_AS]->setStatusTip(tr("Save patch to a different file"));

    actions[ACTION_EXPORT_SELECTION] = new QAction(tr("E&xport selection as patch..."), this);
    actions[ACTION_EXPORT_SELECTION]->setStatusTip(tr("Save the currently selected circuits into a new patch file"));
    actions[ACTION_EXPORT_SELECTION]->setEnabled(false);

    actions[ACTION_UPLOAD_TO_DROID] = new QAction(ICON("exit_to_app"), tr("Activate!"), this);
    actions[ACTION_UPLOAD_TO_DROID]->setShortcut(QKeySequence(tr("F9")));

    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    #ifdef Q_OS_MACOS
    QString title = tr("Reveal in finder");
    #else
    QString title = tr("Re&veal in explorer");
    #endif
    actions[ACTION_OPEN_ENCLOSING_FOLDER] = new QAction(title, this);
    actions[ACTION_OPEN_ENCLOSING_FOLDER]->setStatusTip(tr("Open the folder where the current patch is located."));
    #endif
    actions[ACTION_INTEGRATE_PATCH] = new QAction(NOICON("extension"), tr("&Integrate other patch as new section"), this);
    actions[ACTION_INTEGRATE_PATCH]->setShortcut(QKeySequence(tr("Ctrl+I")));
    actions[ACTION_INTEGRATE_PATCH]->setStatusTip(tr("Load another patch, add that as a new section after the currently selected section "
                               "and try to move the controls, inputs and outputs of that patch to unused "
                               "jacks and controlls"));

    actions[ACTION_JUMP_TO_NEXT_PROBLEM] = new QAction(NOICON("warning"), tr("&Jump to next problem"), this);
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setShortcut(QKeySequence(tr("F6")));
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setStatusTip(tr("Jump to the next problem in your patch. You "
                                             "need to fix all these problems before you can load "
                                             "the patch to your master."));

    actions[ACTION_PROBLEMS] = new QAction(NOICON("warning"), tr("Problems"), this);
    actions[ACTION_PROBLEMS]->setStatusTip(tr("You have problems in your patch. You "
                                             "need to fix all these problems before you can load "
                                             "the patch to your master."));

    actions[ACTION_QUIT] = new QAction(tr("&Quit"), this);
    actions[ACTION_QUIT]->setShortcuts(QKeySequence::Quit);
    actions[ACTION_QUIT]->setStatusTip(tr("Quit DROID Forge"));

    actions[ACTION_PATCH_PROPERTIES] = new QAction(NOICON("dns"), tr("&Patch properties..."), this);
    actions[ACTION_PATCH_PROPERTIES]->setShortcut(QKeySequence(tr("Ctrl+.")));

    actions[ACTION_UNDO] = new QAction(NOICON("undo"), tr("&Undo"), this);
    actions[ACTION_UNDO]->setShortcuts(QKeySequence::Undo);
    actions[ACTION_UNDO]->setStatusTip(tr("Undo last edit action"));

    actions[ACTION_REDO] = new QAction(NOICON("redo"), tr("&Redo"), this);
    actions[ACTION_REDO]->setShortcuts(QKeySequence::Redo);
    actions[ACTION_REDO]->setStatusTip(tr("Redo last edit action"));

    actions[ACTION_CUT] = new QAction(NOICON("content_cut"), tr("C&ut"), this);
    actions[ACTION_CUT]->setShortcuts(QKeySequence::Cut);
    actions[ACTION_CUT]->setStatusTip(tr("Cut selection to clipboard"));

    actions[ACTION_COPY] = new QAction(NOICON("content_copy"), tr("&Copy"), this);
    actions[ACTION_COPY]->setShortcuts(QKeySequence::Copy);
    actions[ACTION_COPY]->setStatusTip(tr("Copy selected stuff to clipboard"));

    actions[ACTION_PASTE] = new QAction(NOICON("content_paste"), tr("&Paste"), this);
    actions[ACTION_PASTE]->setShortcuts(QKeySequence::Paste);
    actions[ACTION_PASTE]->setStatusTip(tr("Paste contents from clipboard"));
    actions[ACTION_PASTE]->setEnabled(false); // enabled by clipboard

    actions[ACTION_PASTE_SMART] = new QAction(tr("&Paste smart"), this);
    actions[ACTION_PASTE_SMART]->setShortcut(QKeySequence(tr("Shift+Ctrl+V")));
    actions[ACTION_PASTE_SMART]->setStatusTip(tr("Paste circuits from clipboard but remap registers and internal connections "
                                 "in order to avoid conflicts."));
    actions[ACTION_PASTE_SMART]->setEnabled(false); // enabled by clipboard

    actions[ACTION_EXPAND_ARRAY] = new QAction(tr("Expand jack array by one"), this);
    actions[ACTION_EXPAND_ARRAY]->setShortcut(QKeySequence(tr("Ctrl+E")));
    actions[ACTION_EXPAND_ARRAY]->setStatusTip(tr("Works only for parameter lines with arrays like pitch1...16. "
                                                     "Grows that array by creating one more line of that type with "
                                                     "increasing number."));

    actions[ACTION_EXPAND_ARRAY_MAX] = new QAction(tr("Expand jack array to max"), this);
    actions[ACTION_EXPAND_ARRAY_MAX]->setShortcut(QKeySequence(tr("Shift+Ctrl+E")));
    actions[ACTION_EXPAND_ARRAY_MAX]->setStatusTip(tr("Works only for parameter lines with arrays like pitch1...16. "
                                                  "Grows that array to its maximum size by creating all remaining "
                                                  "parameter lines."));

    actions[ACTION_ADD_MISSING_JACKS] = new QAction(tr("Add remaining jacks"), this);
    actions[ACTION_ADD_MISSING_JACKS]->setShortcut(QKeySequence(tr("Ctrl+U")));
    actions[ACTION_ADD_MISSING_JACKS]->setStatusTip(tr("Adds all jacks of this circuit that are not already defined."));

    actions[ACTION_REMOVE_UNDEFINED_JACKS] = new QAction(tr("Remove undefined jacks"), this);
    actions[ACTION_REMOVE_UNDEFINED_JACKS]->setShortcut(QKeySequence(tr("Shift+Ctrl+U")));
    actions[ACTION_REMOVE_UNDEFINED_JACKS]->setStatusTip(tr("Rmoves all jacks in your definition that have not assigned values to them"));

    actions[ACTION_SELECT_ALL] = new QAction(tr("Select all"), this);
    actions[ACTION_SELECT_ALL]->setShortcut(QKeySequence(tr("Ctrl+A")));
    actions[ACTION_SELECT_ALL]->setStatusTip(tr("Select all circuits in the current section"));

    actions[ACTION_DISABLE] = new QAction(tr("&Disable"), this);
    actions[ACTION_DISABLE]->setShortcut(QKeySequence(tr("#")));

    actions[ACTION_ENABLE] = new QAction(tr("&Enable"), this);
    actions[ACTION_ENABLE]->setShortcut(QKeySequence(tr("#")));

    actions[ACTION_SORT_JACKS] = new QAction(NOICON("filter_list"), tr("Sort jack assignments"), this);
    actions[ACTION_SORT_JACKS]->setShortcut(QKeySequence(tr("Meta+S")));

    actions[ACTION_NEW_CIRCUIT] = new QAction(ICON("open_in_new"), tr("&New circuit..."), this);
    actions[ACTION_NEW_CIRCUIT]->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));

    actions[ACTION_ADD_JACK] = new QAction(ICON("settings_input_composite"), tr("&New jack"), this);
    actions[ACTION_ADD_JACK]->setShortcut(QKeySequence(tr("Ctrl+N")));

    actions[ACTION_EDIT_VALUE] = new QAction(NOICON("edit"), tr("&Edit element under cursor..."), this);
    actions[ACTION_EDIT_VALUE]->setShortcuts({ QKeySequence(tr("Enter")),
                                    QKeySequence(tr("Return"))});
    actions[ACTION_FOLLOW_CABLE] = new QAction(NOICON("youtube_searched_for"), tr("&Follow internal cable"), this);
    actions[ACTION_FOLLOW_CABLE]->setShortcut(QKeySequence(tr("?")));

    actions[ACTION_RENAME_CABLE] = new QAction(tr("&Rename internal cable"), this);
    actions[ACTION_RENAME_CABLE]->setShortcut(QKeySequence(tr("Ctrl+R")));

    actions[ACTION_START_PATCHING] = new QAction(NOICON("swap_horiz"), tr("Start creating internal cable"), this);
    actions[ACTION_START_PATCHING]->setShortcut(QKeySequence(tr("=")));

    actions[ACTION_FINISH_PATCHING] = new QAction(NOICON("swap_horiz"), tr("Finish creating internal cable"), this);
    actions[ACTION_FINISH_PATCHING]->setShortcut(QKeySequence(tr("=")));
    actions[ACTION_FINISH_PATCHING]->setEnabled(false);

    actions[ACTION_ABORT_PATCHING] = new QAction(NOICON("swap_horiz"), tr("Abort creating internal cable"), this);
    actions[ACTION_ABORT_PATCHING]->setEnabled(false);

    actions[ACTION_EDIT_CIRCUIT_COMMENT] = new QAction(tr("Edit circuit comment..."), this);
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));

    actions[ACTION_EDIT_JACK_COMMENT] = new QAction(tr("Edit jack info..."), this);
    actions[ACTION_EDIT_JACK_COMMENT]->setShortcut(QKeySequence(tr("Alt+C")));

    actions[ACTION_PREVIOUS_SECTION] = new QAction(tr("Previous section"));
    actions[ACTION_PREVIOUS_SECTION]->setShortcut(QKeySequence(tr("Ctrl+Up")));
    actions[ACTION_PREVIOUS_SECTION]->setStatusTip(tr("Switch to the previous section"));

    actions[ACTION_NEXT_SECTION] = new QAction(tr("Next section"));
    actions[ACTION_NEXT_SECTION]->setShortcut(QKeySequence(tr("Ctrl+Down")));
    actions[ACTION_NEXT_SECTION]->setStatusTip(tr("Switch to the next section"));

    actions[ACTION_NEW_PATCH_SECTION] = new QAction(tr("New section..."), this);
    actions[ACTION_NEW_PATCH_SECTION]->setShortcut(QKeySequence(tr("Meta+N")));

    actions[ACTION_DUPLICATE_PATCH_SECTION] = new QAction(tr("Duplicate section..."), this);

    actions[ACTION_RENAME_PATCH_SECTION] = new QAction(tr("Rename section..."), this);

    actions[ACTION_DELETE_PATCH_SECTION] = new QAction(tr("Delete section"), this);

    actions[ACTION_CREATE_SECTION_FROM_SELECTION] = new QAction(tr("Create new section from selection"), this);
    actions[ACTION_CREATE_SECTION_FROM_SELECTION]->setEnabled(false);

    actions[ACTION_MERGE_WITH_PREVIOUS_SECTION] = new QAction(tr("Merge with previous section"));

    actions[ACTION_MERGE_WITH_NEXT_SECTION] = new QAction(tr("Merge with next section"));
    actions[ACTION_MERGE_ALL_SECTIONS] = new QAction(tr("Merge all sections into one"));

    actions[ACTION_RESET_ZOOM] = new QAction(NOICON("zoom_in"), tr("Normal font size"), this);
    actions[ACTION_RESET_ZOOM]->setShortcut(QKeySequence(tr("Ctrl+0")));

    actions[ACTION_ZOOM_IN] = new QAction(NOICON("zoom_in"), tr("Increase font size"), this);
    actions[ACTION_ZOOM_IN]->setShortcuts(QKeySequence::ZoomIn);

    actions[ACTION_ZOOM_OUT] = new QAction(NOICON("zoom_out"), tr("Outcrease font size"), this);
    actions[ACTION_ZOOM_OUT]->setShortcuts(QKeySequence::ZoomOut);

    actions[ACTION_FOLD_UNFOLD] = new QAction(NOICON("format_line_spacing"), tr("Fold / unfold circuit"), this);
    actions[ACTION_FOLD_UNFOLD]->setShortcut(QKeySequence(tr("Space")));

    actions[ACTION_FOLD_UNFOLD_ALL] = new QAction(tr("Fold / unfold all circuits"), this);
    actions[ACTION_FOLD_UNFOLD_ALL]->setShortcut(QKeySequence(tr("Shift+Space")));

    actions[ACTION_ADD_CONTROLLER] = new QAction(ICON("keyboard"), tr("&Add module..."), this);
    actions[ACTION_ADD_CONTROLLER]->setShortcut(QKeySequence(tr("Ctrl+Alt+N")));

    // for (auto action: actions)
    //     action->setShortcutVisibleInContextMenu(false);
}
/*
 * void MainWindow::updateActions()
{
    // File menu

    // Edit menu
    actionPaste->setEnabled(patchView.clipboardFilled());
    actionPasteSmart->setEnabled(patchView.circuitsInClipboard());

    const PatchSectionView *psv = patchView.currentPatchSectionView();
    bool empty = !psv || psv->isEmpty();
    actions[ACTION_ADD_JACK]->setEnabled(!empty);
    actions[ACTION_EDIT_VALUE]->setEnabled(!empty);
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setEnabled(!empty);
    const Atom *atom = 0;
    if (psv)
        atom = psv->currentAtom();
    bool isAtAtom = psv && psv->getCursorPosition().isAtAtom();

    if (psv && patchView.numSections() > 1) {
        actionDeletePatchSection->setText(tr("Delete section '%1'").arg(psv->getTitle()));
        actionDeletePatchSection->setEnabled(true);
    }
    else {
        actionDeletePatchSection->setText(tr("Delete section"));
        actionDeletePatchSection->setEnabled(false);
    }
    actions[ACTION_MOVE_INTO_SECTION]->setEnabled(patchView.circuitsSelected());
}*/
