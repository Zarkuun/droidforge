#ifdef NIXDA
void PatchView::jumpTo(int section, const CursorPosition &curPos)
{
    setCurrentIndex(section);
    currentPatchSectionView()->setCursorPosition(curPos);
    patch->switchCurrentSection(section);
}

void PatchView::connectActions()
{
    qDebug() << "ICH CONNECTED";
    qDebug() << "RETURNS:" <<
    connect(the_actions->action(ACTION_CUT), &QAction::triggered, this, &PatchView::cut);
    qDebug() << "ACTION" << the_actions->action(ACTION_CUT);

    CONNECT_ACTION(ACTION_FOLLOW_INTERNAL_CABLE, &PatchView::followInternalCable);
    CONNECT_ACTION(ACTION_RENAME_INTERNAL_CABLE, &PatchView::renameInternalCable);
    CONNECT_ACTION(ACTION_START_PATCHING, &PatchView::startPatching);
    CONNECT_ACTION(ACTION_FINISH_PATCHING, &PatchView::finishPatching);
    CONNECT_ACTION(ACTION_ABORT_PATCHING, &PatchView::abortPatching);
    CONNECT_ACTION(ACTION_EDIT_CIRCUIT_COMMENT, &PatchView::editCircuitComment);

}


void PatchView::addJack()
{
    if (currentPatchSectionView()->isEmpty())
        return;

    QString circuit = currentPatchSectionView()->currentCircuitName();
    QStringList usedJacks = currentPatchSectionView()->usedJacks();

    QString name = JackChooseDialog::chooseJack(circuit, "", usedJacks);
    if (!name.isEmpty())
        currentPatchSectionView()->addNewJack(name);
}

void PatchView::startPatching()
{
    patching = true;
    patchingStartSection = currentIndex();
    patchingStartPosition = currentPatchSectionView()->getCursorPosition();
    the_forge->cableIndicator()->setPatchingState(true);
    currentPatchSectionView()->updateCursor(); // TODO: This seems to be hack
}

void PatchView::finishPatching()
{
    Q_ASSERT(patching);
    patching = false;
    the_forge->cableIndicator()->setPatchingState(false);

    Q_ASSERT((int)patchingStartSection < count());

    PatchSectionView *startView = (PatchSectionView *)widget(patchingStartSection);
    PatchSection *startSection = patch->section(patchingStartSection);
    CursorPosition startPos = patchingStartPosition;
    const Atom *startAtom = startSection->atomAt(startPos);

    PatchSectionView *endView = currentPatchSectionView();
    PatchSection *endSection = patch->section(currentIndex());
    CursorPosition endPos = currentPatchSectionView()->getCursorPosition();
    const Atom *endAtom = endSection->atomAt(endPos);

    QString cableName;
    if (startAtom && startAtom->isCable())
        cableName = ((AtomCable *)startAtom)->getCable();
    else if (endAtom && endAtom->isCable())
        cableName = ((AtomCable *)endAtom)->getCable();
    else {
        cableName = NameChooseDialog::getName(tr("Create new internal patch cable"), tr("Cable name:"));
        if (cableName == "") {
            return;
        }
        cableName = cableName.toUpper();
    }

    the_forge->registerEdit(tr("creating internal cable '%1'").arg(cableName));
    startSection->setAtomAt(startPos, new AtomCable(cableName));
    endSection->setAtomAt(endPos, new AtomCable(cableName));
    if (startView != endView)
        startView->rebuildPatchSection();
    endView->rebuildPatchSection();
    the_forge->patchHasChanged();
}

void PatchView::abortPatching()
{
    patching = false;
    the_forge->cableIndicator()->setPatchingState(false);
}

void PatchView::followInternalCable()
{
    const Atom *currentAtom = currentPatchSectionView()->currentAtom();
    if (!currentAtom || !currentAtom->isCable())
        return;

    QString name = ((AtomCable *)currentAtom)->getCable();

    bool waitForNext = false;
    bool found = false;

    // First try to find the next atom *after* the current
    Patch::iterator it = patch->begin();
    while (*it) {
        Atom *atom = *it;
        if (atom == currentAtom)
            waitForNext = true;
        else if (waitForNext && atom->isCable() && ((AtomCable *)atom)->getCable() == name) {
            found = true;
            break;
        }
        ++it;
    }

    // Now try from the start of the patch
    if (!found) {
        it = patch->begin();
        while (*it && *it != currentAtom) {
            Atom *atom = *it;
            if (atom->isCable() && ((AtomCable *)atom)->getCable() == name) {
                found = true;
                break;
            }
            ++it;
        }
    }

    if (!found)
        return;

    jumpTo(it.sectionIndex(), it.cursorPosition());
}

void PatchView::reorderSections(int fromindex, int toindex)
{
    the_forge->registerEdit("reordering sections");
    patch->reorderSections(fromindex, toindex);
    the_forge->patchHasChanged();
}

void PatchView::tabContextMenu(int index)
{
    if (QApplication::mouseButtons() == Qt::RightButton) {
        // TODO: Damit sich das Menü auf die richtige Sektion bezieht,
        // muss ich sie selektieren, bevor ich das Menü aufmachen kann!
        // Sonst brauch ich alle Slots doppelt und auch die Aktionen.
        setCurrentIndex(index);

        QMenu *menu = new QMenu(this);
        ADD_ACTION(ACTION_NEW_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_DUPLICATE_PATCH_SECTION, menu);
        ADD_ACTION(ACTION_MOVE_INTO_SECTION, menu);
        ADD_ACTION(ACTION_MERGE_WITH_LEFT_SECTION, menu);
        ADD_ACTION(ACTION_MERGE_WITH_RIGHT_SECTION, menu);
        ADD_ACTION(ACTION_DELETE_PATCH_SECTION, menu);
        menu->popup(QCursor::pos());
    }
}
#endif
