#include "patchoperator.h"
#include "colorscheme.h"
#include "globals.h"
#include "mainwindow.h"
#include "parseexception.h"
#include "updatehub.h"
#include "patchpropertiesdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QProcess>
#include <QTimer>

PatchOperator *the_operator = 0;

PatchOperator::PatchOperator(PatchEditEngine *patch, QString initialFilename)
    : patch(patch)
{
    Q_ASSERT(the_operator == 0);
    the_operator = this;

    CONNECT_ACTION(ACTION_QUIT, &PatchOperator::quit);
    CONNECT_ACTION(ACTION_NEW, &PatchOperator::newPatch);
    CONNECT_ACTION(ACTION_OPEN, &PatchOperator::open);
    CONNECT_ACTION(ACTION_SAVE, &PatchOperator::save);
    CONNECT_ACTION(ACTION_SAVE_AS, &PatchOperator::saveAs);
    #if (defined Q_OS_MACOS || defined Q_OS_WIN)
    CONNECT_ACTION(ACTION_OPEN_ENCLOSING_FOLDER, &PatchOperator::openEnclosingFolder);
    #endif
    CONNECT_ACTION(ACTION_EXPORT_SELECTION, &PatchOperator::exportSelection);
    CONNECT_ACTION(ACTION_INTEGRATE_PATCH, &PatchOperator::integrate);
    CONNECT_ACTION(ACTION_UNDO, &PatchOperator::undo);
    CONNECT_ACTION(ACTION_REDO, &PatchOperator::redo);
    CONNECT_ACTION(ACTION_PATCH_PROPERTIES, &PatchOperator::editProperties);
    CONNECT_ACTION(ACTION_CONFIGURE_COLORS, &PatchOperator::configureColors);

    // Events that we create
    connect(this, &PatchOperator::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchOperator::clipboardChanged, the_hub, &UpdateHub::changeClipboard);
    connect(this, &PatchOperator::selectionChanged, the_hub, &UpdateHub::changeSelection);
    connect(this, &PatchOperator::sectionSwitched, the_hub, &UpdateHub::switchSection);
    connect(this, &PatchOperator::cursorMoved, the_hub, &UpdateHub::moveCursor);
    connect(this, &PatchOperator::patchingChanged, the_hub, &UpdateHub::changePatching);

    QSettings settings;
    if (initialFilename == "" && settings.contains("lastfile"))
        initialFilename = settings.value("lastfile").toString();
    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [this,initialFilename] () { this->loadFile(initialFilename, FILE_MODE_LOAD);});
}

void PatchOperator::newPatch()
{
    if (!checkModified())
        return;

    patch->startFromScratch();
    patch->addSection(new PatchSection());
    patch->commit(tr("creating new patch"));
    emit patchModified();
}
void PatchOperator::createRecentFileActions(QMenu *fileMenu)
{
    QMenu *menu = fileMenu->addMenu(tr("Open recent file"));
    QStringList recentFiles = getRecentFiles();
    for (qsizetype i=0; i<recentFiles.count(); i++) {
        QFileInfo fi(recentFiles[i]);
        if (!fi.exists())
            continue;
        QAction *action = new QAction(fi.baseName(), this);
        QString path = fi.absoluteFilePath();
        connect(action, &QAction::triggered, this, [this, path]() { this->loadFile(path, FILE_MODE_LOAD); });
        menu->addAction(action);
    }
}
bool PatchOperator::checkModified()
{
    // TODO: Aktivieren vor Release!!
    return true;

    // TODO rackview modified
    if (patch->isModified()) {
        QMessageBox box(
                    QMessageBox::Warning,
                    tr("Your patch is modified!"),
                    tr("Do you want to save your changes before you proceed?"),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    the_forge);
        int ret = box.exec();
        switch (ret) {
        case QMessageBox::Save:
            // TODO: Check success of saving!
            save();
            return true;

        case QMessageBox::Discard:
            return true;

        default: // QMessageBox::Cancel:
            return false;
        }
    }
    else
        return true;
}
void PatchOperator::quit()
{
    if (checkModified())
        exit(0);
}
void PatchOperator::jumpTo(int sectionIndex, const CursorPosition &pos)
{
    int oldSection = patch->currentSectionIndex();
    patch->setCursorTo(sectionIndex, pos);
    if (section()->currentCircuit()->isFolded()) {
        section()->currentCircuit()->setFold(false);
        emit patchModified();
    }
    else if (patch->currentSectionIndex() != oldSection)
        emit sectionSwitched();
    else
        emit cursorMoved();
}
void PatchOperator::loadFile(const QString &filePath, int how)
{
    if (FILE_MODE_LOAD && !checkModified())
        return;

    try {
        addToRecentFiles(filePath);
        if (how == FILE_MODE_LOAD)
            loadPatch(filePath);
        else
            integratePatch(filePath);

    }
    catch (ParseException &e) {
        QMessageBox box;
        box.setText(MainWindow::tr("Cannot load ") + filePath);
        box.setInformativeText(e.toString());
        box.setStandardButtons(QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Critical);
        // TODO: Size of message box?
        // box.setBaseSize(QSize(600, 220));
        box.exec();
    }
}
void PatchOperator::open()
{
    if (!checkModified())
        return;

    QString filePath = QFileDialog::getOpenFileName(the_forge);
    if (!filePath.isEmpty()) {
        loadFile(filePath, FILE_MODE_LOAD);
        setLastFilePath(filePath);
    }
}
void PatchOperator::integrate()
{
    QString filePath = QFileDialog::getOpenFileName(the_forge);
    if (!filePath.isEmpty())
        loadFile(filePath, FILE_MODE_INTEGRATE);
}
void PatchOperator::exportSelection()
{
    QString filePath = QFileDialog::getSaveFileName(
                the_forge,
                tr("Export selection into new patch"),
                "",
                tr("DROID patch files (*.ini)"));
    if (!filePath.isEmpty()) {
        Patch *patch = section()->getSelectionAsPatch();
        // TODO: Error if it failed
        patch->saveToFile(filePath);
        delete patch;
        addToRecentFiles(filePath);
    }
}
void PatchOperator::save()
{
    if (patch->getFilePath().isEmpty())
        saveAs();
    else {
        patch->save(patch->getFilePath());
        emit patchModified(); // mod flag
    }
}
void PatchOperator::saveAs()
{
    QString newFilePath = QFileDialog::getSaveFileName(
                the_forge,
                tr("Save patch to new file"),
                patch->getFilePath(),
                tr("DROID patch files (*.ini)"));
    if (!newFilePath.isEmpty()) {
        patch->save(newFilePath);
        patch->setFilePath(newFilePath);
        emit patchModified(); // mod flag
        addToRecentFiles(newFilePath);
        setLastFilePath(newFilePath);
    }
}
void PatchOperator::openEnclosingFolder()
{
    QFileInfo fileinfo(patch->getFilePath());
    openDirInFinder(fileinfo.absoluteFilePath());
}
void PatchOperator::openDirInFinder(const QString &filename)
{
#ifdef Q_OS_MACOS
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \""+filename+"\"";
    args << "-e";
    args << "end tell";
    QProcess::startDetached("osascript", args);

#endif
#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(filename);
    QProcess::startDetached("explorer", args);
#endif
}
void PatchOperator::editProperties()
{
    if (PatchPropertiesDialog::editPatchProperties(patch)) {
        patch->commit(tr("editing patch properties"));
        emit patchModified();
    }
}
void PatchOperator::undo()
{
    qDebug("ICH UNDOE");
    if (patch->undoPossible()) {
        patch->undo();
        emit patchModified();
    }
}
void PatchOperator::redo()
{
    if (patch->redoPossible()) {
        patch->redo();
        emit patchModified();
    }
}
void PatchOperator::setLastFilePath(const QString &path)
{
    QSettings settings;
    settings.setValue("lastfile", path);
}
QStringList PatchOperator::getRecentFiles()
{
    QSettings settings;
    return settings.value("recentfiles").toStringList();
}
void PatchOperator::addToRecentFiles(const QString &path)
{
    QFileInfo fi(path);
    QStringList files = getRecentFiles();
    files.removeAll(fi.absoluteFilePath());
    files.prepend(fi.absoluteFilePath());
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();
    QSettings settings;
    settings.setValue("recentfiles", files);
}
void PatchOperator::loadPatch(const QString &aFilePath)
{
    Patch newPatch;
    parser.parse(aFilePath, &newPatch); // throws exception
    // reached if parsing was successfull
    patch->startFromScratch();
    patch->setFilePath(aFilePath);
    newPatch.cloneInto(patch);
    patch->commit(tr("loading patch"));
    emit patchModified();
}
void PatchOperator::integratePatch(const QString &aFilePath)
{
    Patch otherpatch;
    parser.parse(aFilePath, &otherpatch); // may throw exceptions

    Patch *newPatch = patch->clone();
    if (interactivelyRemapRegisters(&otherpatch, newPatch)) {
        newPatch->integratePatch(&otherpatch);
        newPatch->cloneInto(patch);
        patch->commit(tr("integrating other patch '%1'").arg(otherpatch.getTitle()));
        delete newPatch;
        emit patchModified();
    }
}
bool PatchOperator::interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch)
{
    // ontoPatch is that patch that the "otherPatch" want's to get integrated
    // to. In situations where the otherPatch does not have controllers, that
    // would be added to the target patch, ontoPatch is 0 and we can simply
    // use the "live" patch, since it will not be changed.
    if (ontoPatch == 0)
        ontoPatch = patch;

    // Phase 1: If the other patch defines controllers, we can add these
    // controllers to our patch (and shift all references, of course)
    const QStringList &controllers = otherPatch->allControllers();
    if (!controllers.isEmpty())
    {
        int reply = QMessageBox::question(
                    the_forge,
                    TR("Controllers"),
                    TR("The integrated patch contains controller definitions: %1. "
                       "Do you want to add these definitions to your patch?")
                       .arg(controllers.join(" ").toUpper()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            int numExistingControllers = ontoPatch->numControllers();
            for (auto &c: controllers)
                ontoPatch->addController(c);
            otherPatch->shiftControllerNumbers(-1, numExistingControllers);
        }
    }

    // TODO: Hier gibt's nochn Bug. Beispiel. Ich hab im otherpatch I1, I2 und I3.
    // Und im alten auch. Es wird aber nur I1->I4 gelegt. I2, und I3 sagt er kann
    // er nicht.

    // Phase 2: Remap non-existing or conflicting registers.
    RegisterList availableRegisters;
    ontoPatch->collectAvailableRegisterAtoms(availableRegisters);
    RegisterList occupiedRegisters;
    ontoPatch->collectUsedRegisterAtoms(occupiedRegisters);
    RegisterList neededRegisters;
    otherPatch->collectUsedRegisterAtoms(neededRegisters);
    RegisterList atomsToRemap;
    for (auto &reg: neededRegisters) {
        if (occupiedRegisters.contains(reg) || !availableRegisters.contains(reg)) {
            atomsToRemap.append(reg);
        }
        else
            occupiedRegisters.append(reg); // now occupied
    }
    if (atomsToRemap.count()) {
        int reply = QMessageBox::question(
                    the_forge,
                    TR("Register conflicts"),
                    TR("Some of the register references in the integrated patch either do not exist in your "
                       "current rack definition or are already occupied. Shall I try to find useful replacements "
                       "for those?\n\n%1").arg(atomsToRemap.toString()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            RegisterList remapFrom;
            RegisterList remapTo;
            RegisterList remapped;

            for (auto& toRemap: atomsToRemap) {
                for (auto &candidate: availableRegisters) {
                    if (occupiedRegisters.contains(candidate))
                        continue;
                    if (toRemap.getRegisterType() != candidate.getRegisterType())
                        continue;
                    remapFrom.append(toRemap);
                    remapTo.append(candidate);
                    occupiedRegisters.append(candidate);
                    remapped.append(toRemap);
                }
            }

            for (auto& atom: remapped)
                atomsToRemap.removeAll(atom);

            // Apply this remapping
            for (unsigned i=0; i<remapFrom.size(); i++)
                otherPatch->remapRegister(remapFrom[i], remapTo[i]);

            // Phase 2b: Remaining un remapped registers
            if (!atomsToRemap.isEmpty()) {
                int reply = QMessageBox::question(
                            the_forge,
                            TR("Register conflicts"),
                            TR("For some register references I could not find a valid replacement in your patch. "
                               "Shall I remove these references (otherwise I would just leave them as "
                               "they are and you check yourselves later)?\n\n%1").arg(atomsToRemap.toString()),
                            QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes);

                if (reply == QMessageBox::Cancel)
                    return false;

                else if (reply == QMessageBox::Yes) {
                    otherPatch->removeRegisterReferences(
                                atomsToRemap,
                                ControllerRemovalDialog::INPUT_REMOVE,
                                ControllerRemovalDialog::OUTPUT_REMOVE);
                }
            }
        }
    }

    // Phase 3: Cables
    // TODO: Alle Kabel des otherpatch sammeln. KOnflikte finden.
    // Wenn es welche gibt, fragen:
    // - umbenennen
    // - lassen
    // - abbrechen
    return true;
}

void PatchOperator::configureColors()
{
    if (the_colorscheme->isVisible())
        the_colorscheme->hide();
    else
        the_colorscheme->show();
}
