#include "patchoperator.h"
#include "colorscheme.h"
#include "globals.h"
#include "mainwindow.h"
#include "parseexception.h"
#include "updatehub.h"
#include "patchpropertiesdialog.h"
#include "sourcecodeeditor.h"
#include "atomcable.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <QClipboard>
#include <QStorageInfo>
#include <QThread>

// #include <CoreMIDI/MIDIServices.h>

#ifdef Q_OS_WIN
//#include <stdio.h>
#include <windows.h>
// #include <Setupapi.h>
// #include <winioctl.h>
// #include <winioctl.h>
// #include <cfgmgr32.h>
// #include <fileapi.h>
bool PatchOperator::ejectSDWindows(const QString letter)
{
    QString device_path = QStringLiteral("%1:\\").arg(letter);
    QString error_string;
    const char* temp = "\\\\.\\";
    char device_path1[10] = { 0 };
    memcpy(device_path1, temp, strlen(temp));
    QByteArray dp = device_path.toLocal8Bit();
    device_path1[4] = dp.at(0);
    device_path1[5] = dp.at(1);
    HANDLE handleDevice = CreateFileA(device_path1, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    bool is_handle_invalid = (handleDevice == INVALID_HANDLE_VALUE);
    if (is_handle_invalid)
    {
        error_string = "Device is not connection to system!";
        qDebug() << GetLastError();
        return false;
    }
    // Do this in a loop until a timeout period has expired
    const int try_lock_volume_count = 3;
    int try_count = 0;
    for (; try_count < try_lock_volume_count; ++try_count)
    {
        DWORD dwBytesReturned;
        if (!DeviceIoControl(handleDevice, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &dwBytesReturned, nullptr))
        {
            qDebug() << "Device is using....." << try_count;
            break;
        }
        QThread::sleep(1);
    }
    if (try_count == try_lock_volume_count)
    {
        error_string = "Device is using, try again later";
        CloseHandle(handleDevice);
        return false;
    }
    DWORD  dwBytesReturned = 0;
    PREVENT_MEDIA_REMOVAL PMRBuffer;
    PMRBuffer.PreventMediaRemoval = FALSE;
    if (!DeviceIoControl(handleDevice, IOCTL_STORAGE_MEDIA_REMOVAL, &PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL), nullptr, 0, &dwBytesReturned, nullptr))
    {
        error_string = QStringLiteral("Unmount failed! error code:%1").arg(GetLastError());
        qDebug() << "DeviceIoControl IOCTL_STORAGE_MEDIA_REMOVAL failed:" << GetLastError();
        CloseHandle(handleDevice);
        return false;
    }
    long   bResult = 0;
    DWORD retu = 0;
    bResult = DeviceIoControl(handleDevice, IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0, &retu, nullptr);
    if (!bResult)
    {
        error_string = QStringLiteral("Disconnect IGU failed! error code:%1").arg(GetLastError());
        CloseHandle(handleDevice);
        qDebug() << "Disconnect IGU IoControl failed error:" << GetLastError();
        return false;
    }
    CloseHandle(handleDevice);
    return true;
}

#endif

PatchOperator *the_operator = 0;

PatchOperator::PatchOperator(PatchEditEngine *patch, QString initialFilename)
    : patch(patch)
    , sdCardPresent(false)
    , x7Present(false)
{
    Q_ASSERT(the_operator == 0);
    the_operator = this;

    CONNECT_ACTION(ACTION_QUIT, &PatchOperator::quit);
    CONNECT_ACTION(ACTION_UPLOAD_TO_DROID, &PatchOperator::upload);
    CONNECT_ACTION(ACTION_TOOLBAR_UPLOAD_TO_DROID, &PatchOperator::upload);
    CONNECT_ACTION(ACTION_SAVE_TO_SD, &PatchOperator::saveToSD);
    CONNECT_ACTION(ACTION_TOOLBAR_SAVE_TO_SD, &PatchOperator::saveToSD);
    CONNECT_ACTION(ACTION_NEW, &PatchOperator::newPatch);
    CONNECT_ACTION(ACTION_NEW_WITH_SAME_RACK, &PatchOperator::newPatchWithSameRack);
    CONNECT_ACTION(ACTION_TOOLBAR_NEW, &PatchOperator::newPatchWithSameRack);
    CONNECT_ACTION(ACTION_OPEN, &PatchOperator::open);
    CONNECT_ACTION(ACTION_CLEAR_RECENT_FILES, &PatchOperator::clearRecentFiles);
    CONNECT_ACTION(ACTION_TOOLBAR_OPEN, &PatchOperator::open);
    CONNECT_ACTION(ACTION_SAVE, &PatchOperator::save);
    CONNECT_ACTION(ACTION_TOOLBAR_SAVE, &PatchOperator::save);
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
    CONNECT_ACTION(ACTION_MOVE_CIRCUIT_UP, &PatchOperator::moveCircuitUp);
    CONNECT_ACTION(ACTION_MOVE_CIRCUIT_DOWN, &PatchOperator::moveCircuitDown);

    CONNECT_ACTION(ACTION_EDIT_CIRCUIT_SOURCE, &PatchOperator::editCircuitSource);
    CONNECT_ACTION(ACTION_EDIT_SECTION_SOURCE, &PatchOperator::editSectionSource);
    CONNECT_ACTION(ACTION_EDIT_PATCH_SOURCE, &PatchOperator::editPatchSource);
    CONNECT_ACTION(ACTION_BARE_PATCH_SOURCE, &PatchOperator::barePatchSource);
    CONNECT_ACTION(ACTION_ABORT_ALL_ACTIONS, &PatchOperator::abortAllActions);

    CONNECT_ACTION(ACTION_FIX_LED_MISMATCH, &PatchOperator::fixLEDMismatch);

    // Events that we create
    connect(this, &PatchOperator::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchOperator::clipboardChanged, the_hub, &UpdateHub::changeClipboard);
    connect(this, &PatchOperator::selectionChanged, the_hub, &UpdateHub::changeSelection);
    connect(this, &PatchOperator::sectionSwitched, the_hub, &UpdateHub::switchSection);
    connect(this, &PatchOperator::cursorMoved, the_hub, &UpdateHub::moveCursor);
    connect(this, &PatchOperator::patchingChanged, the_hub, &UpdateHub::changePatching);
    connect(this, &PatchOperator::droidStateChanged, the_hub, &UpdateHub::changeDroidState);
    connect(this, &PatchOperator::patchingChanged, the_hub, &UpdateHub::changePatching);

    // Event that we are interested in
    connect(the_hub, &UpdateHub::patchModified, this, &PatchOperator::modifyPatch);

    QSettings settings;
    if (initialFilename == "" && settings.contains("lastfile"))
        initialFilename = settings.value("lastfile").toString();
    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [this,initialFilename] () { this->loadFile(initialFilename, FILE_MODE_LOAD);});
    else
        QTimer::singleShot(0, this, [this] () { this->newPatch();});

    QTimer *sdTimer = new QTimer(this);
    connect(sdTimer, &QTimer::timeout, this, &PatchOperator::updateSDAndX7State);
    sdTimer->start(SD_CHECK_INTERVAL);

    QClipboard *clipboard = QGuiApplication::clipboard();
    connect(clipboard, &QClipboard::dataChanged, this, &PatchOperator::globalClipboardChanged);
}
void PatchOperator::newPatch()
{
    if (!checkModified())
        return;

    patch->startFromScratch();
    patch->addSection(new PatchSection());
    patch->commit(tr("creating new patch"));
    setLastFilePath("");
    emit patchModified();
}
void PatchOperator::newPatchWithSameRack()
{
    if (!checkModified())
        return;

    QStringList controllers = patch->allControllers();

    patch->startFromScratch();
    for (auto controller: controllers)
        patch->addController(controller);

    patch->addSection(new PatchSection());
    patch->commit(tr("creating new patch with same rack"));
    setLastFilePath("");
    emit patchModified();
}
void PatchOperator::createRecentFileActions(QMenu *menu)
{
    menu->clear();
    PatchParser parser;
    QStringList recentFiles = getRecentFiles();
    for (qsizetype i=0; i<recentFiles.count(); i++) {
        Patch *patch = parser.parseFile(recentFiles[i]);
        if (patch) {
            QFileInfo fi(recentFiles[i]);
            QString title = fi.fileName();
            if (patch->hasTitle())
                title += " - " + patch->getTitle();
            QAction *action = new QAction(title, this);
            delete patch;
            QString path = fi.absoluteFilePath();
            connect(action, &QAction::triggered, this, [this, path]() { this->loadFile(path, FILE_MODE_LOAD); });
            menu->addAction(action);
        }
    }
    menu->addSeparator();
    ADD_ACTION(ACTION_CLEAR_RECENT_FILES, menu);
    ACTION(ACTION_CLEAR_RECENT_FILES)->setEnabled(recentFiles.count() > 0);
    recentFilesMenu = menu;
}
bool PatchOperator::checkModified()
{
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
            save();
            return !patch->isModified();

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
void PatchOperator::upload()
{
    if (patch->isModified())
        save();

    QString error = midiHost.sendPatch(patch);
    if (error != "") {
        QMessageBox::critical(
                    the_forge,
                    tr("Cannot send patch via MIDI"),
                    error,
                    QMessageBox::Ok);
    }
}
void PatchOperator::saveToSD()
{
    QString dirPath = sdCardDir();
    if (dirPath == "") {
        QMessageBox::critical(
                    the_forge,
                    tr("Cannot find memory card"),
                    tr("There is no DROID SD card mounted.\n\nIf it has been "
                       "ejected, remove and reinsert it. Note: it must be a "
                       "microSD card that has already been used in your DROID master."),
                    QMessageBox::Ok);
        return;
    }

    QDir dir(dirPath);
    QFileInfo droidIni(dir, DROID_PATCH_FILENAME);
    if (!patch->saveToFile(droidIni.absoluteFilePath()))
    {
        QMessageBox::critical(
                    the_forge,
                    tr("Cannot write %1").arg(QString(DROID_PATCH_FILENAME)),
                    tr("Error writing %1").arg(droidIni.absoluteFilePath()),
                    QMessageBox::Ok);
        return;
    }

#ifdef Q_OS_WIN
    if (ejectSDWindows(dir.absolutePath().mid(0, 1)))
    {
        sdCardPresent = false;
        emit droidStateChanged();
    }
    else {
        QMessageBox::warning(
                    the_forge,
                    tr("Could not eject SD card"),
                    tr("An error occurred while ejecting the SD card"),
                    QMessageBox::Ok);
    }
#else
    QProcess process;
    QStringList arguments;
    arguments << "umount";
    arguments << dir.absolutePath();
    process.start("diskutil", arguments);
    bool success = process.waitForFinished(MAC_UMOUNT_TIMEOUT_MS);

    if (!success) { // never happened ever.
        QMessageBox::warning(
                    the_forge,
                    tr("Could not eject SD card"),
                    tr("Timeout while trying to safely eject the SD card."),
                    QMessageBox::Ok);
    }
    else if (process.exitStatus() != QProcess::NormalExit) { // neither happened
        QString error = process.readAll(); // always empty
        QMessageBox::warning(
                    the_forge,
                    tr("Could not eject SD card"),
                    tr("An error occurred while ejecting the SD card:\n%1").arg(error),
                    QMessageBox::Ok);
    }
    else if (dir.exists()) {
        QMessageBox::warning(
                    the_forge,
                    tr("Could not eject SD card"),
                    tr("I have saved your patch to the SD card.\n\n"
                       "However, I could not eject it.\n\n"
                       "Probably the card is in use by some other application. "
                       "Better go and check and eject the card with Finder before "
                       "you remove it from the card reader."),
                    QMessageBox::Ok);
    }
    else {
        // QString all = process.readAll();
        sdCardPresent = false;
        emit droidStateChanged();
    }
#endif
}
QString PatchOperator::sdCardDir() const
{
    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
        if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
            if (isDroidVolume(storage.rootPath())) {
                return storage.rootPath();
            }
        }
    }
    return "";
}
bool PatchOperator::isDroidVolume(const QString &rootPath) const
{
    QDir dir(rootPath);
    return dir.exists("DROIDCAL.BIN") || dir.exists("DROIDSTA.BIN");
}
void PatchOperator::updateSDAndX7State()
{
    bool oldSDState = sdCardPresent;
    sdCardPresent = sdCardDir() != "";

    if (oldSDState != sdCardPresent) {
        emit droidStateChanged();
        return;
    }

    bool oldX7State = x7Present;
    x7Present = midiHost.x7Connected();
    if (oldX7State != x7Present)
        emit droidStateChanged();
}
void PatchOperator::loadFile(const QString &filePath, int how)
{
    if (how == FILE_MODE_LOAD && !checkModified())
        return;

    // Use auto backup file if present
    QString backupPath = backupFilePath(filePath);
    bool restore = false;
    if (how == FILE_MODE_LOAD)
    {
        QFileInfo info(backupPath);
        if (info.exists() && info.size() > 0) {
            QString date = info.birthTime().toString();
            int reply = QMessageBox::question(
                        the_forge,
                        tr("Auto backup detected"),
                        tr("There is an automatic backup file of the patch that you "
                           "are going to load. Maybe the Forge or your computer crashed "
                           "last time.\n\n"
                           "Path: %1\n"
                           "Size: %2 bytes\n"
                           "Date: %3\n\n"
                           "Do you want to to restore that backup? If you choose "
                           "\"No\", I will delete that backup and go ahead.\n")
                        .arg(backupPath).arg(info.size()).arg(date),
                        QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                        QMessageBox::Yes);
            if (reply == QMessageBox::Cancel)
                return;
            else if (reply == QMessageBox::No)
            {
                QFile backup(backupPath);
                backup.remove();
            }
            else
                restore = true;
        }
    }

    try {
        addToRecentFiles(filePath);
        if (how == FILE_MODE_LOAD) {
            loadPatch(filePath);
            if (restore) {
                try {
                    restore = false;
                    restoreBackup(backupPath);
                }
                catch (ParseException &e) {
                    QMessageBox::critical(
                                the_forge,
                                tr("Cannot restore backup"),
                                tr("Parse error in the backup file. The backup cannot be restored. I will use the original file."),
                                QMessageBox::Ok);
                }
            }
        }
        else
            integratePatch(filePath);
        emit patchModified();
    }
    catch (ParseException &e) {
        // If the loading of the file has not worked, but we have a backup,
        // we rename the backup into the actual file name and try again
        if (restore) { // initial load has not worked
            QFile b(backupPath);
            QFile f(filePath);
            f.remove();
            if (b.rename(f.fileName())) {
                try {
                    loadPatch(filePath);
                    emit patchModified();
                    return;
                }
                catch (ParseException &innerE) {
                    e = innerE;
                }
            }
        }

        QMessageBox box;
        box.setText(MainWindow::tr("Cannot load ") + filePath);
        box.setInformativeText(e.toString());
        box.setStandardButtons(QMessageBox::Cancel);
        box.setDefaultButton(QMessageBox::Cancel);
        box.setIcon(QMessageBox::Critical);
        box.exec();
    }
}
void PatchOperator::open()
{
    if (!checkModified())
        return;

    QString filePath = QFileDialog::getOpenFileName(
                the_forge, "", "", "DROID patches (*.ini)");

    if (!filePath.isEmpty()) {
        loadFile(filePath, FILE_MODE_LOAD);
    }
}
void PatchOperator::clearRecentFiles()
{
    QSettings settings;
    QStringList emptyList;
    settings.setValue("recentfiles", emptyList);
    createRecentFileActions(recentFilesMenu);
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
        if (!patch->saveToFile(filePath))
        {
            QMessageBox::warning(
                        the_forge,
                        tr("Error"),
                        tr("There was an error saving your selection to disk."));
        }
        else
            addToRecentFiles(filePath);
        delete patch;
    }
}
void PatchOperator::save()
{
    if (patch->getFilePath().isEmpty())
        saveAs();
    else
        saveAndCheck(patch->getFilePath());
}
void PatchOperator::saveAs()
{
    QString newFilePath = QFileDialog::getSaveFileName(
                the_forge,
                tr("Save patch to new file"),
                patch->getFilePath(),
                tr("DROID patch files (*.ini)"));
    if (!newFilePath.isEmpty()) {
        if (saveAndCheck(newFilePath))
            addToRecentFiles(newFilePath);
    }
}
bool PatchOperator::saveAndCheck(QString path)
{
    if (patch->save(path)) {
        patch->setFilePath(path);
        setLastFilePath(path);
        emit patchModified(); // mod flag has changed
        removeBackup();
        return true;
    }
    else
        QMessageBox::warning(
                    the_forge,
                    tr("Error"),
                    tr("There was an error saving your patch to disk"));
    return false;
}
QString PatchOperator::backupFilePath(QString path)
{
    return path + ".save";
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
    createRecentFileActions(recentFilesMenu);
}
void PatchOperator::loadPatch(const QString &aFilePath)
{
    Patch newPatch;
    parser.parseFile(aFilePath, &newPatch); // throws exception

    // Move contents of new patch into "our" patch without
    // invalidating its pointer
    patch->startFromScratch();
    patch->setFilePath(aFilePath);
    newPatch.cloneInto(patch);

    // Beware: a patch *must* have at least one section. If we load
    // an empty patch, we crash if there is no section
    if (patch->numSections() == 0)
        patch->addSection(new PatchSection());

    patch->commit(tr("loading patch"));
    setLastFilePath(aFilePath);
}
void PatchOperator::restoreBackup(const QString &backupPath)
{
    Patch backupPatch;
    parser.parseFile(backupPath, &backupPatch); // throws exception
    backupPatch.cloneInto(patch);
    if (patch->numSections() == 0)
        patch->addSection(new PatchSection());
    patch->commit(tr("restoring backup"));
    emit patchModified();
}
void PatchOperator::createBackup()
{
    QString backupname = backupFilePath(patch->getFilePath());
    patch->saveToFile(backupname);
}
void PatchOperator::removeBackup()
{
    QString backupname = backupFilePath(patch->getFilePath());
    QFile file(backupname);
    file.remove();
}
void PatchOperator::integratePatch(const QString &aFilePath)
{
    Patch otherpatch;
    parser.parseFile(aFilePath, &otherpatch); // may throw exceptions

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

    // Phase 2: Remap non-existing or conflicting registers.
    RegisterList availableRegisters;
    ontoPatch->collectAvailableRegisterAtoms(availableRegisters);
    RegisterList occupiedRegisters;
    ontoPatch->collectUsedRegisterAtoms(occupiedRegisters);
    RegisterList neededRegisters;
    otherPatch->collectUsedRegisterAtoms(neededRegisters);

    // Create a list of registers to remap and a list of registers
    // that are occupied (in the new configuration already)
    RegisterList atomsToRemap;
    for (auto &reg: neededRegisters) {
        // registers need to be remapped if they are not existing in our
        // current controller configuration or if they are already
        // occupied.
        if (occupiedRegisters.contains(reg) || !availableRegisters.contains(reg))
            atomsToRemap.append(reg);
        // Not need to be remapped: then they *now* occupy a register
        // and are not available for remapping
        else
            occupiedRegisters.append(reg); // now occupied
    }

    if (atomsToRemap.count()) {
        int reply = QMessageBox::question(
                    the_forge,
                    TR("Register conflicts"),
                    TR("Some of the register references in the integrated patch either do not exist in your "
                       "current rack definition or are already occupied. Shall I try to find useful replacements "
                       "for those?\n\n%1").arg(atomsToRemap.toSmartString()),
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

                    if (toRemap.getRegisterType() != candidate.getRegisterType())
                        continue;

                    if (occupiedRegisters.contains(candidate))
                        continue;

                    remapFrom.append(toRemap);
                    remapTo.append(candidate);
                    occupiedRegisters.append(candidate);
                    remapped.append(toRemap);
                    break;
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
                            tr("Register conflicts"),
                            tr("For some register references I could not find a valid replacement in your patch. "
                               "Shall I remove these references (otherwise I would just leave them as "
                               "they are and you check yourselves later)?\n\n%1").arg(atomsToRemap.toSmartString()),
                            QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes);

                if (reply == QMessageBox::Cancel)
                    return false;

                else if (reply == QMessageBox::Yes) {
                    otherPatch->removeRegisterReferences(atomsToRemap);
                }
            }
        }
    }

    // Phase 3: Cables
    // Cables that are internally to otherpatch are renamed, if they
    // have a conflict. Cables that come "in" or "out" the otherpatch
    // will be renamed, if the user whishes to do so.

    QStringList existingCables = ontoPatch->allCables();

    // Loop through all cables of the newly integrated patch
    for (auto cable: otherPatch->allCables())
    {
        // Does this cable already exist in the existing patch?
        if (existingCables.contains(cable))
        {
            // Check if that cable is pure internally. We consider
            // it as internally if it has at least one in and one
            // out usage.
            int asInput = 0;
            int asOutput = 0;
            otherPatch->findCableConnections(cable, asInput, asOutput);
            bool isInternally = asInput > 0 && asOutput > 0;
            bool rename = true;
            if (!isInternally) {
                int ontoAsInput = 0;
                int ontoAsOutput = 0;
                otherPatch->findCableConnections(cable, ontoAsInput, ontoAsOutput);

                // A cable that's just read from and is not internally obviously
                // is better not renamed. It probably want's to read from the output
                // of the other part of the patch
                if (asInput && !asOutput)
                    rename = false;

                else if (asOutput && ontoAsOutput) {
                    int reply = QMessageBox::question(
                                the_forge,
                                tr("Cable conflict"),
                                tr("The internal patch cable \"%1\" is used as output both by "
                                   "the existing and the integrated patch. This will result in a "
                                   "problem.\n\nShall I rename that cable in order to avoid fear, "
                                   "uncertainty and doubt?").arg(cable),
                                QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes);

                    if (reply == QMessageBox::Cancel)
                        return false;

                    else if (reply == QMessageBox::No)
                        rename = false;
                }
            }
            if (rename) {
                QString newName = AtomCable::nextCableName(cable);
                while (ontoPatch->cableExists(newName) || otherPatch->cableExists(newName))
                    newName = AtomCable::nextCableName(newName);
                otherPatch->renameCable(cable, newName);
            }
        }
    }
    return true;
}
bool PatchOperator::isPatchingFrom(const CursorPosition &pos) const
{
    return patch->isPatchingFrom(pos);
}
void PatchOperator::configureColors()
{
    if (the_colorscheme->isVisible())
        the_colorscheme->hide();
    else
        the_colorscheme->show();
}
void PatchOperator::moveCircuitUp()
{
    clearSelection();
    int id = section()->currentCircuitId();
    section()->swapCircuits(id-1, id);
    patch->commit(tr("moving circuit up"));
    emit patchModified();
}
void PatchOperator::moveCircuitDown()
{
    int id = section()->currentCircuitId();
    section()->swapCircuits(id, id+1);
    patch->commit(tr("moving circuit down"));
    emit patchModified();
}
void PatchOperator::editPatchSource()
{
    Patch *parsed = editSource(tr("Patch source code"), patch->toString());
    if (parsed) {
        parsed->cloneInto(patch);
        patch->commit(tr("editing patch source code"));
        delete parsed;
        emit patchModified();
    }
}
void PatchOperator::editSectionSource()
{
    QString title = tr("Source code of section '%1'").arg(patch->currentSection()->getNonemptyTitle());
    Patch *parsed = editSource(title, section()->toString());
    if (parsed) {
        int sectionIndex = patch->currentSectionIndex();
        patch->removeSection(sectionIndex);
        patch->insertSection(sectionIndex, parsed->section(0)->clone());
        section()->sanitizeCursor();
        patch->commit(tr("editing section source code"));
        delete parsed;
        emit patchModified();
    }
}
void PatchOperator::editCircuitSource()
{
    // Add one empty line, otherwise we will loose the first comment
    // line of the circuit if it has any
    Patch *parsed = editSource(tr("Circuit source code"), "\n" + section()->currentCircuit()->toString());
    if (parsed) {
        int circuitIndex = section()->currentCircuitId();
        section()->deleteCircuit(circuitIndex);
        for (auto circuit: parsed->section(0)->getCircuits())
            section()->insertCircuit(circuitIndex++, circuit->clone());
        section()->sanitizeCursor();
        patch->commit(tr("editing circuit source code"));
        delete parsed;
        emit patchModified();
    }
}
void PatchOperator::barePatchSource()
{
    showSource(tr("Patch source without comments"), patch->toCleanString());
}
void PatchOperator::fixLEDMismatch()
{
    section()->currentCircuit()->fixLEDMismatches();
    section()->sanitizeCursor();
    patch->commit(tr("fixing LED mismatches"));
    emit patchModified();
}
void PatchOperator::globalClipboardChanged()
{
    the_clipboard->copyFromGlobalClipboard();
    emit clipboardChanged();
}
void PatchOperator::modifyPatch()
{
    if (patch->isModified())
        createBackup();
    else
        removeBackup();
}
Patch *PatchOperator::editSource(const QString &title, QString oldSource)
{
    SourceCodeEditor editor(title, oldSource, the_forge, false /* readonly */);
    PatchParser parser;
    while (true) {
        if (!editor.edit())
            return 0; // User aborted
        try {
            QString newSource = editor.getEditedText();
            Patch parsed;
            parser.parseString(newSource, &parsed);
            return parsed.clone();
        }
        catch (ParseException &e) {
            QMessageBox::critical(
                        the_forge,
                        tr("Parse error"),
                        e.toString(),
                        QMessageBox::Ok);
        }
    }
}
void PatchOperator::showSource(const QString &title, QString source)
{
    SourceCodeEditor editor(title, source, the_forge, true /* readonly */);
    editor.exec();
}
void PatchOperator::clearSelection()
{
    section()->clearSelection();
    emit selectionChanged();
}
void PatchOperator::abortAllActions()
{
    if (patch->isPatching()) {
        patch->stopPatching();
        emit patchingChanged();
    }

    bool changed = false;
    for (unsigned i=0; i<patch->numSections(); i++) {
        PatchSection *s = patch->section(i);
        if (s->getSelection()) {
            s->clearSelection();
            changed = true;
        }
    }
    if (changed)
        emit selectionChanged();
}

#ifdef Q_OS_WIN_KAPUT
#include <stdio.h>
#include <windows.h>
#include <Setupapi.h>
#include <winioctl.h>
#include <winioctl.h>
#include <cfgmgr32.h>
#include <fileapi.h>

//-------------------------------------------------
DEVINST GetDrivesDevInstByDeviceNumber(long DeviceNumber, UINT DriveType, wchar_t* szDosDeviceName);
//-------------------------------------------------

//-------------------------------------------------
bool PatchOperator::ejectSDWindows(char DriveLetter)
{

    wchar_t szRootPath[] = L"F:\\";   // "X:\"  -> for GetDriveType
    szRootPath[0] = DriveLetter;

    wchar_t szDevicePath[] = L"F:";   // "X:"   -> for QueryDosDevice
    szDevicePath[0] = DriveLetter;

    wchar_t szVolumeAccessPath[] = L"\\\\.\\F:";   // "\\.\X:"  -> to open the volume
    szVolumeAccessPath[4] = DriveLetter;

    long DeviceNumber = -1;

    // open the storage volume
    HANDLE hVolume = CreateFile(szVolumeAccessPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
    if (hVolume == INVALID_HANDLE_VALUE) {
        return 1;
    }

    // get the volume's device number
    STORAGE_DEVICE_NUMBER sdn;
    DWORD dwBytesReturned = 0;
    long res = DeviceIoControl(hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
    if (res) {
        DeviceNumber = sdn.DeviceNumber;
    }
    CloseHandle(hVolume);

    if (DeviceNumber == -1) {
        return 1;
    }

    // get the drive type which is required to match the device numbers correctely
    UINT DriveType = GetDriveType(szRootPath);

    // get the dos device name (like \device\floppy0) to decide if it's a floppy or not - who knows a better way?
    wchar_t szDosDeviceName[MAX_PATH];
    res = QueryDosDevice(szDevicePath, szDosDeviceName, MAX_PATH);
    if (!res) {
        return 1;
    }

    // get the device instance handle of the storage volume by means of a SetupDi enum and matching the device number
    DEVINST DevInst = GetDrivesDevInstByDeviceNumber(DeviceNumber, DriveType, szDosDeviceName);

    if (DevInst == 0) {
        return 1;
    }

    PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;
    WCHAR VetoNameW[MAX_PATH];
    VetoNameW[0] = 0;
    bool bSuccess = false;

    // get drives's parent, e.g. the USB bridge, the SATA port, an IDE channel with two drives!
    DEVINST DevInstParent = 0;
    res = CM_Get_Parent(&DevInstParent, DevInst, 0);

    for (long tries = 1; tries <= 3; tries++) { // sometimes we need some tries...

        VetoNameW[0] = 0;

        // CM_Query_And_Remove_SubTree doesn't work for restricted users
        //res = CM_Query_And_Remove_SubTreeW(DevInstParent, &VetoType, VetoNameW, MAX_PATH, CM_REMOVE_NO_RESTART); // CM_Query_And_Remove_SubTreeA is not implemented under W2K!
        //res = CM_Query_And_Remove_SubTreeW(DevInstParent, NULL, NULL, 0, CM_REMOVE_NO_RESTART);  // with messagebox (W2K, Vista) or balloon (XP)

        res = CM_Request_Device_EjectW(DevInstParent, &VetoType, VetoNameW, MAX_PATH, 0);
        //res = CM_Request_Device_EjectW(DevInstParent, NULL, NULL, 0, 0); // with messagebox (W2K, Vista) or balloon (XP)

        bSuccess = (res == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown);
        if (bSuccess) {
            break;
        }

        Sleep(500); // required to give the next tries a chance!
    }

    if (bSuccess) {
        printf("Success\n\n");
        return 0;
    }

    printf("failed\n");

    printf("Result=0x%2X\n", res);

    if (VetoNameW[0]) {
        printf("VetoName=%ws)\n\n", VetoNameW);
    }
    return 1;
}
//-----------------------------------------------------------



//----------------------------------------------------------------------
// returns the device instance handle of a storage volume or 0 on error
//----------------------------------------------------------------------
DEVINST GetDrivesDevInstByDeviceNumber(long DeviceNumber, UINT DriveType, wchar_t* szDosDeviceName)
{
    bool IsFloppy = FALSE; // (strstr(szDosDeviceName, "\\Floppy") != NULL); // who knows a better way?

    GUID* guid;

    switch (DriveType) {
    case DRIVE_REMOVABLE:
        if (IsFloppy) {
            guid = (GUID*)&GUID_DEVINTERFACE_FLOPPY;
        }
        else {
            guid = (GUID*)&GUID_DEVINTERFACE_DISK;
        }
        break;
    case DRIVE_FIXED:
        guid = (GUID*)&GUID_DEVINTERFACE_DISK;
        break;
    case DRIVE_CDROM:
        guid = (GUID*)&GUID_DEVINTERFACE_CDROM;
        break;
    default:
        return 0;
    }

    // Get device interface info set handle for all devices attached to system
    HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return 0;
    }

    // Retrieve a context structure for a device interface of a device information set
    DWORD dwIndex = 0;
    long res;

    BYTE Buf[1024];
    PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buf;
    SP_DEVICE_INTERFACE_DATA         spdid;
    SP_DEVINFO_DATA                  spdd;
    DWORD                            dwSize;

    spdid.cbSize = sizeof(spdid);

    while (true) {
        res = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guid, dwIndex, &spdid);
        if (!res) {
            break;
        }

        dwSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, NULL, 0, &dwSize, NULL); // check the buffer size

        if (dwSize != 0 && dwSize <= sizeof(Buf)) {

            pspdidd->cbSize = sizeof(*pspdidd); // 5 Bytes!

            ZeroMemory(&spdd, sizeof(spdd));
            spdd.cbSize = sizeof(spdd);

            long res = SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, pspdidd, dwSize, &dwSize, &spdd);
            if (res) {

                // in case you are interested in the USB serial number:
                // the device id string contains the serial number if the device has one,
                // otherwise a generated id that contains the '&' char...
                /*
                DEVINST DevInstParent = 0;
                CM_Get_Parent(&DevInstParent, spdd.DevInst, 0);
                char szDeviceIdString[MAX_PATH];
                CM_Get_Device_ID(DevInstParent, szDeviceIdString, MAX_PATH, 0);
                printf("DeviceId=%s\n", szDeviceIdString);
                */

                // open the disk or cdrom or floppy
                HANDLE hDrive = CreateFile(pspdidd->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                if (hDrive != INVALID_HANDLE_VALUE) {
                    // get its device number
                    STORAGE_DEVICE_NUMBER sdn;
                    DWORD dwBytesReturned = 0;
                    res = DeviceIoControl(hDrive, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
                    if (res) {
                        if (DeviceNumber == (long)sdn.DeviceNumber) {  // match the given device number with the one of the current device
                            CloseHandle(hDrive);
                            SetupDiDestroyDeviceInfoList(hDevInfo);
                            return spdd.DevInst;
                        }
                    }
                    CloseHandle(hDrive);
                }
            }
        }
        dwIndex++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    return 0;
}
#endif