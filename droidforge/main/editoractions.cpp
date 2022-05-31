#include "editoractions.h"
#include "updatehub.h"
#include "clipboard.h"

EditorActions *the_actions = 0;

EditorActions::EditorActions(QObject *parent)
    : QObject{parent}
    , patch(0)
{
    Q_ASSERT(the_actions == 0);
    the_actions = this;
    createActions();

    // Events that we are interested in
    connect(the_hub, &UpdateHub::sectionSwitched, this, &EditorActions::switchSection);
    connect(the_hub, &UpdateHub::patchChanged, this, &EditorActions::changePatch);
    connect(the_hub, &UpdateHub::patchModified, this, &EditorActions::modifyPatch);
    connect(the_hub, &UpdateHub::clipboardChanged, this, &EditorActions::changeClipboard);
    connect(the_hub, &UpdateHub::selectionChanged, this, &EditorActions::changeSelection);
}

void EditorActions::changePatch(VersionedPatch *newPatch)
{
    patch = newPatch;
    modifyPatch();
}

void EditorActions::modifyPatch()
{
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

    // There alwas *must* be at least one section
    actions[ACTION_DELETE_PATCH_SECTION]->setEnabled(patch->numSections() >= 2);
    actions[ACTION_MERGE_ALL_SECTIONS]->setEnabled(patch->numSections() >= 2);
    switchSection();
}

void EditorActions::switchSection()
{
    int sectionIndex = patch->currentSectionIndex();
    int numSections = patch->numSections();
    actions[ACTION_MERGE_WITH_PREVIOUS_SECTION]->setEnabled(sectionIndex > 0);
    actions[ACTION_MERGE_WITH_NEXT_SECTION]->setEnabled(sectionIndex+1 < numSections);
}

void EditorActions::changeClipboard()
{
    actions[ACTION_PASTE]->setEnabled(!the_clipboard->isEmpty());
    actions[ACTION_PASTE_SMART]->setEnabled(the_clipboard->numCircuits());
}

void EditorActions::changeSelection(const Selection *selection)
{
    actions[ACTION_EXPORT_SELECTION]->setEnabled(selection && selection->isCircuitSelection());
    actions[ACTION_MOVE_INTO_SECTION]->setEnabled(selection && selection->isCircuitSelection());
}

void EditorActions::createActions()
{
    actions[ACTION_NEW] = new QAction(icon("settings_input_composite"), tr("&New..."), this);
    actions[ACTION_NEW]->setShortcut(QKeySequence(tr("Ctrl+Shift+Alt+N")));
    actions[ACTION_NEW]->setStatusTip(tr("Create a new patch from scratch"));

    actions[ACTION_OPEN] = new QAction(icon("open_in_browser"), tr("&Open..."), this);
    actions[ACTION_OPEN]->setShortcuts(QKeySequence::Open);
    actions[ACTION_OPEN]->setStatusTip(tr("Open an existing patch"));

    actions[ACTION_SAVE] = new QAction(icon("save"), tr("&Save..."), this);
    actions[ACTION_SAVE]->setShortcuts(QKeySequence::Save);
    actions[ACTION_SAVE]->setStatusTip(tr("Save patch to file"));

    actions[ACTION_SAVE_AS] = new QAction(tr("Save &as..."), this);
    actions[ACTION_SAVE_AS]->setShortcuts(QKeySequence::SaveAs);
    actions[ACTION_SAVE_AS]->setStatusTip(tr("Save patch to a different file"));

    actions[ACTION_EXPORT_SELECTION] = new QAction(tr("E&xport selection as patch..."), this);
    actions[ACTION_EXPORT_SELECTION]->setStatusTip(tr("Save the currently selected circuits into a new patch file"));
    actions[ACTION_EXPORT_SELECTION]->setEnabled(false);

    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    #ifdef Q_OS_MACOS
    QString title = tr("Reveal in finder");
    #else
    QString title = tr("Re&veal in explorer");
    #endif
    actions[ACTION_OPEN_ENCLOSING_FOLDER] = new QAction(title, this);
    actions[ACTION_OPEN_ENCLOSING_FOLDER]->setStatusTip(tr("Open the folder where the current patch is located."));
    #endif
    actions[ACTION_INTEGRATE_PATCH] = new QAction(icon("extension"), tr("&Integrate other patch as new section"), this);
    actions[ACTION_INTEGRATE_PATCH]->setShortcut(QKeySequence(tr("Ctrl+I")));
    actions[ACTION_INTEGRATE_PATCH]->setStatusTip(tr("Load another patch, add that as a new section after the currently selected section "
                               "and try to move the controls, inputs and outputs of that patch to unused "
                               "jacks and controlls"));

    actions[ACTION_JUMP_TO_NEXT_PROBLEM] = new QAction(icon("warning"), tr("&Jump to next problem"), this);
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setShortcut(QKeySequence(tr("F6")));
    actions[ACTION_JUMP_TO_NEXT_PROBLEM]->setStatusTip(tr("Jump to the next problem in your patch. You "
                                             "need to fix all these problems before you can load "
                                             "the patch to your master."));

    actions[ACTION_QUIT] = new QAction(tr("&Quit"), this);
    actions[ACTION_QUIT]->setShortcuts(QKeySequence::Quit);
    actions[ACTION_QUIT]->setStatusTip(tr("Quit DROID Forge"));

    actions[ACTION_PATCH_PROPERTIES] = new QAction(icon("dns"), tr("&Patch properties..."), this);
    actions[ACTION_PATCH_PROPERTIES]->setShortcut(QKeySequence(tr("Ctrl+.")));

    actions[ACTION_UNDO] = new QAction(icon("undo"), tr("&Undo"), this);
    actions[ACTION_UNDO]->setShortcuts(QKeySequence::Undo);
    actions[ACTION_UNDO]->setStatusTip(tr("Undo last edit action"));

    actions[ACTION_REDO] = new QAction(icon("redo"), tr("&Redo"), this);
    actions[ACTION_REDO]->setShortcuts(QKeySequence::Redo);
    actions[ACTION_REDO]->setStatusTip(tr("Redo last edit action"));

    actions[ACTION_CUT] = new QAction(icon("cut"), tr("C&ut"), this);
    actions[ACTION_CUT]->setShortcuts(QKeySequence::Cut);
    actions[ACTION_CUT]->setStatusTip(tr("Cut selection to clipboard"));

    actions[ACTION_COPY] = new QAction(icon("copy"), tr("&Copy"), this);
    actions[ACTION_COPY]->setShortcuts(QKeySequence::Copy);
    actions[ACTION_COPY]->setStatusTip(tr("Copy selected stuff to clipboard"));

    actions[ACTION_PASTE] = new QAction(icon("paste"), tr("&Paste"), this);
    actions[ACTION_PASTE]->setShortcuts(QKeySequence::Paste);
    actions[ACTION_PASTE]->setStatusTip(tr("Paste contents from clipboard"));
    actions[ACTION_PASTE]->setEnabled(false); // enabled by clipboard

    actions[ACTION_PASTE_SMART] = new QAction(tr("&Paste smart"), this);
    actions[ACTION_PASTE_SMART]->setShortcut(QKeySequence(tr("Shift+Ctrl+V")));
    actions[ACTION_PASTE_SMART]->setStatusTip(tr("Paste circuits from clipboard but remap registers and internal connections "
                                 "in order to avoid conflicts."));
    actions[ACTION_PASTE_SMART]->setEnabled(false); // enabled by clipboard

    actions[ACTION_NEW_CIRCUIT] = new QAction(icon("open_in_new"), tr("&New circuit..."), this);
    actions[ACTION_NEW_CIRCUIT]->setShortcut(QKeySequence(tr("Shift+Ctrl+N")));

    actions[ACTION_ADD_JACK] = new QAction(icon("settings_input_composite"), tr("&New jack..."), this);
    actions[ACTION_ADD_JACK]->setShortcut(QKeySequence(tr("Ctrl+N")));

    actions[ACTION_EDIT_VALUE] = new QAction(icon("edit"), tr("&Edit element under cursor..."), this);
    actions[ACTION_EDIT_VALUE]->setShortcuts({ QKeySequence(tr("Enter")),
                                    QKeySequence(tr("Return"))});
    actions[ACTION_FOLLOW_INTERNAL_CABLE] = new QAction(icon("youtube_searched_for"), tr("&Follow internal cable"), this);
    actions[ACTION_FOLLOW_INTERNAL_CABLE]->setShortcut(QKeySequence(tr("?")));

    actions[ACTION_RENAME_INTERNAL_CABLE] = new QAction(tr("&Rename internal cable"), this);
    actions[ACTION_RENAME_INTERNAL_CABLE]->setShortcut(QKeySequence(tr("Ctrl+R")));

    actions[ACTION_START_PATCHING] = new QAction(icon("swap_horiz"), tr("Start creating internal cable"), this);
    actions[ACTION_START_PATCHING]->setShortcut(QKeySequence(tr("=")));

    actions[ACTION_FINISH_PATCHING] = new QAction(icon("swap_horiz"), tr("Finish creating internal cable"), this);
    actions[ACTION_FINISH_PATCHING]->setShortcut(QKeySequence(tr("=")));

    actions[ACTION_ABORT_PATCHING] = new QAction(icon("swap_horiz"), tr("Abort creating internal cable"), this);

    actions[ACTION_EDIT_CIRCUIT_COMMENT] = new QAction(tr("Edit circuit comment..."), this);
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setShortcut(QKeySequence(tr("Shift+Ctrl+C")));
    actions[ACTION_EDIT_CIRCUIT_COMMENT]->setShortcutVisibleInContextMenu(true);

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

    actions[ACTION_MOVE_INTO_SECTION] = new QAction(tr("Move selection into new section"), this);
    actions[ACTION_MOVE_INTO_SECTION]->setEnabled(false);

    actions[ACTION_MERGE_WITH_PREVIOUS_SECTION] = new QAction(tr("Merge with previous section"));

    actions[ACTION_MERGE_WITH_NEXT_SECTION] = new QAction(tr("Merge with next section"));

    actions[ACTION_MERGE_ALL_SECTIONS] = new QAction(tr("Merge all sections into one"));

    actions[ACTION_RESET_ZOOM] = new QAction(icon("zoom_in"), tr("Normal font size"), this);
    actions[ACTION_RESET_ZOOM]->setShortcut(QKeySequence(tr("Ctrl+0")));

    actions[ACTION_ZOOM_IN] = new QAction(icon("zoom_in"), tr("Increase font size"), this);
    actions[ACTION_ZOOM_IN]->setShortcuts(QKeySequence::ZoomIn);

    actions[ACTION_ZOOM_OUT] = new QAction(icon("zoom_out"), tr("Outcrease font size"), this);
    actions[ACTION_ZOOM_OUT]->setShortcuts(QKeySequence::ZoomOut);

    actions[ACTION_ADD_CONTROLLER] = new QAction(icon("keyboard"), tr("&New controller..."), this);
    actions[ACTION_ADD_CONTROLLER]->setShortcut(QKeySequence(tr("Ctrl+Alt+N")));

    for (auto action: actions)
        action->setShortcutVisibleInContextMenu(true);
}


QIcon EditorActions::icon(QString what) const
{
    return QIcon(":/images/icons/white/" + what + ".png");
}

/*
 * void MainWindow::updateActions()
{
    // File menu
    actionSave->setEnabled(patch->isModified());
    actionOpenEnclosingFolder->setEnabled(!filePath.isEmpty());

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

    actions[ACTION_FOLLOW_INTERNAL_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_RENAME_INTERNAL_CABLE]->setEnabled(atom && atom->isCable());
    actions[ACTION_START_PATCHING]->setVisible(isAtAtom && !patchView.isPatching());
    actions[ACTION_FINISH_PATCHING]->setVisible(isAtAtom && patchView.isPatching());
    actions[ACTION_ABORT_PATCHING]->setVisible(patchView.isPatching());

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
