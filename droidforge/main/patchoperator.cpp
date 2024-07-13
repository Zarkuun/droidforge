#include "patchoperator.h"
#include "colorscheme.h"
#include "globals.h"
#include "mainwindow.h"
#include "parseexception.h"
#include "rewritecablesdialog.h"
#include "updatehub.h"
#include "patchpropertiesdialog.h"
#include "sourcecodeeditor.h"
#include "atomcable.h"
#include "mainwindow.h"
#include "utilities.h"
#include "windowlist.h"
#include "namechoosedialog.h"
#include "tuning.h"
#include "memoryanalysiswindow.h"
#include "patchgeneratordialog.h"
#include "hintdialog.h"
#include "waitforsddialog.h"

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

PatchOperator::PatchOperator(MainWindow *mainWindow, PatchEditEngine *patch,
                             QString initialFilename, const Patch *initialRack)
    : mainWindow(mainWindow)
    , patch(patch)
    , sdCardPresent(false)
    , statusDumpPresent(false)
    , x7Present(false)
    , lastShownStatusDumpNr(-1)
    , untitledBackupNumber(0)
{
    dumpsMenu = new QMenu(tr("Status dumps"));
    updateStatusDumpsMenu(false);

    CONNECT_ACTION(ACTION_QUIT, &PatchOperator::quit);
    CONNECT_ACTION(ACTION_CLOSE_WINDOW, &PatchOperator::close);
    CONNECT_ACTION(ACTION_UPLOAD_TO_DROID, &PatchOperator::upload);
    CONNECT_ACTION(ACTION_TOOLBAR_UPLOAD_TO_DROID, &PatchOperator::upload);
    CONNECT_ACTION(ACTION_SAVE_TO_SD, &PatchOperator::saveToSD);
    CONNECT_ACTION(ACTION_TOOLBAR_SAVE_TO_SD, &PatchOperator::saveToSD);
    CONNECT_ACTION(ACTION_UPGRADE_MASTER_FIRMWARE, &PatchOperator::upgradeMasterFirmware);
    CONNECT_ACTION(ACTION_NEW, &PatchOperator::newPatch);
    CONNECT_ACTION(ACTION_NEW_WITH_SAME_RACK, &PatchOperator::newPatchWithSameRack);
    CONNECT_ACTION(ACTION_TOOLBAR_NEW, &PatchOperator::newPatchWithSameRack);
    CONNECT_ACTION(ACTION_OPEN, &PatchOperator::open);
    CONNECT_ACTION(ACTION_OPEN_IN_NEW_WINDOW, &PatchOperator::openInNewWindow);
    CONNECT_ACTION(ACTION_NEW_WINDOW, &PatchOperator::newWindow);
    CONNECT_ACTION(ACTION_NEW_WINDOW_WITH_SAME_RACK, &PatchOperator::newWindowWithSameRack);
    CONNECT_ACTION(ACTION_CLEAR_RECENT_FILES, &PatchOperator::clearRecentFiles);
    CONNECT_ACTION(ACTION_TOOLBAR_OPEN, &PatchOperator::open);
    CONNECT_ACTION(ACTION_SAVE, &PatchOperator::save);
    CONNECT_ACTION(ACTION_TOOLBAR_SAVE, &PatchOperator::save);
    connect(ACTION(ACTION_SAVE_ALL), &QAction::triggered, the_windowlist, &WindowList::saveAll);
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
    CONNECT_ACTION(ACTION_EXPORT_COMPRESSED_PATCH, &PatchOperator::exportCompressedPatch);
    CONNECT_ACTION(ACTION_MEMORY_ANALYSIS, &PatchOperator::patchMemoryAnalysis);
    CONNECT_ACTION(ACTION_ABORT_ALL_ACTIONS, &PatchOperator::abortAllActions);

    CONNECT_ACTION(ACTION_FIX_LED_MISMATCH, &PatchOperator::fixLEDMismatch);
    CONNECT_ACTION(ACTION_REWRITE_CABLE_NAMES, &PatchOperator::rewriteCableNames);
    CONNECT_ACTION(ACTION_SET_BOOKMARK, &PatchOperator::setBookmark);
    CONNECT_ACTION(ACTION_JUMP_TO_BOOKMARK, &PatchOperator::jumpToBookmark);
    CONNECT_ACTION(ACTION_EXPAND_ARRAY, &PatchOperator::expandArray);
    CONNECT_ACTION(ACTION_EXPAND_ARRAY_MAX, &PatchOperator::expandArrayMax);
    CONNECT_ACTION(ACTION_RENAME_CABLE, &PatchOperator::renameCable);

    // Events that we create
    connect(this, &PatchOperator::patchModified, mainWindow->theHub(), &UpdateHub::modifyPatch);
    connect(this, &PatchOperator::selectionChanged, mainWindow->theHub(), &UpdateHub::changeSelection);
    connect(this, &PatchOperator::sectionSwitched, mainWindow->theHub(), &UpdateHub::switchSection);
    connect(this, &PatchOperator::cursorMoved, mainWindow->theHub(), &UpdateHub::moveCursor);
    connect(this, &PatchOperator::patchingChanged, mainWindow->theHub(), &UpdateHub::changePatching);
    connect(this, &PatchOperator::droidStateChanged, mainWindow->theHub(), &UpdateHub::changeDroidState);
    connect(this, &PatchOperator::patchingChanged, mainWindow->theHub(), &UpdateHub::changePatching);

    // Event that we are interested in
    connect(mainWindow->theHub(), &UpdateHub::patchModified, this, &PatchOperator::modifyPatch);

    if (!initialFilename.isEmpty())
        QTimer::singleShot(0, this, [this,initialFilename] () { this->loadFile(initialFilename, FILE_MODE_LOAD);});
    else if (initialRack) {
        clearWithControllersFromOtherRack(initialRack);
        QTimer::singleShot(0, this, [this] () { this->newPatchWithSameRack();});
    }
    else
        QTimer::singleShot(0, this, [this] () { this->newPatch();});

    if (!pollSD()) {
        QTimer *sdTimerSansPolling = new QTimer(this);
        connect(sdTimerSansPolling, &QTimer::timeout, this, &PatchOperator::updateSDAndX7State);
        sdTimerSansPolling->start(SD_CHECK_INTERVAL_SANS_POLLING);
    }
    else {
        // reset SDCard path in settings. In case of path change.
        QSettings settings;
        settings.setValue(SD_PATH_SETTINGS_KEY_NAME, "");

        QTimer *sdTimer = new QTimer(this);
        connect(sdTimer, &QTimer::timeout, this, &PatchOperator::updateSDAndX7State);
        sdTimer->start(SD_CHECK_INTERVAL);
    }
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
    the_windowlist->update();
}
void PatchOperator::newPatchWithSameRack()
{
    if (!checkModified())
        return;
    clearWithControllersFromOtherRack(patch);
    setLastFilePath("");
    emit patchModified();
    the_windowlist->update();
}
void PatchOperator::clearWithControllersFromOtherRack(const Patch *otherPatch)
{
    QStringList controllers = otherPatch->allControllers();

    patch->startFromScratch();
    for (auto controller: controllers)
        patch->addController(controller);

    patch->addSection(new PatchSection());
    patch->commit(tr("creating new patch with same rack"));
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
            connect(action, &QAction::triggered, this, [this, path]() { this->openRecentFile(path); });
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
                    tr("Your patch \"%1\" is modified.\n\n"
                       "Do you want to save your changes before you proceed?").arg(mainWindow->patchTitle()),
                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                    mainWindow);
        int ret = box.exec();
        switch (ret) {
        case QMessageBox::Save:
            save();
            return !patch->isModified();

        case QMessageBox::Discard:
            removeBackup();
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
void PatchOperator::expandArray(bool max)
{
    Circuit *circuit = section()->currentCircuit();
    JackAssignment *ja = section()->currentJackAssignment();
    CursorPosition curPos = section()->cursorPosition();

    // Starting from the *current* index within the jack array find
    // the next hole. Then add the new jack right after the last
    // one before the hole (creating a sane sort order).
    const JackAssignment *jaToClone = ja;

    while (true) {
        QString jackName = ja->jackName();
        QString next = circuit->nextJackArrayName(jackName, ja->jackType() == JACKTYPE_INPUT);
        if (next == "")
            break;

        JackAssignment *newJa = jaToClone->clone();
        newJa->incrementForExpansion(patch);
        newJa->setJackName(next);
        jaToClone = newJa;

        // Insert the new jack right after that with the previous index.
        QString prefix = circuit->prefixOfJack(jackName);
        unsigned thisIndex = next.mid(prefix.length()).toUInt();
        if (thisIndex > 1) {
            QString prevName = prefix + QString::number(thisIndex - 1);
            for (unsigned i=0; i<circuit->numJackAssignments(); i++) {
                if (circuit->jackAssignment(i)->jackName() == prevName) {
                    curPos.row = i;
                    break;
                }
            }
        }

        curPos.row++;
        circuit->insertJackAssignment(newJa, curPos.row);
        section()->setCursor(curPos);
        if (!max)
            break;
    }
    patch->commit(tr("expanding parameter array"));
    emit patchModified();
}
void PatchOperator::expandArrayMax()
{
    expandArray(true);
}
void PatchOperator::renameCable()
{
    const Atom *atom = patch->currentAtom();
    if (!atom || !atom->isCable())
        return;

    QString oldName = ((AtomCable *)atom)->getCable();
    QString newName = NameChooseDialog::getReName(
                tr("Rename internal cable '%1'").arg(oldName),
                tr("New name:"),
                oldName,
                true /* force upper case */);
    if (newName == oldName)
        return;

    newName = newName.toUpper();
    QStringList all = patch->allCables();
    if (all.contains(newName)) {
        int reply = QMessageBox::warning(
                    mainWindow,
                    tr("Conflict"),
                    tr("There already is a cable with this name. Choosing the name '%1'"
                       "will join these cables and make all according inputs and outputs "
                       "connected.\n\n"
                       "Do you want want to join both cables?").arg(newName),
                    QMessageBox::Cancel | QMessageBox::Yes,
                    QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel)
            return;
    }

    patch->renameCable(oldName, newName);
    patch->commit(tr("renaming cable '%1' to '%2'").arg(oldName).arg(newName));
    emit patchModified();
}
void PatchOperator::jumpTo(int sectionIndex, const CursorPosition &pos)
{
    int oldSection = patch->currentSectionIndex();
    bool wasFolded = patch->section(sectionIndex)->circuit(pos.circuitNr)->isFolded();
    patch->setCursorTo(sectionIndex, pos); // unfolds if neccessary

    if (wasFolded)
        emit patchModified();
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
                    mainWindow,
                    tr("Cannot send patch via MIDI"),
                    error,
                    QMessageBox::Ok);
    }
}
void PatchOperator::saveToSD()
{
    if (patch->isModified())
        save();
    QString dirPath = sdCardDir();
    if (dirPath == "") {
        QMessageBox::critical(
                    mainWindow,
                    tr("Cannot find memory card"),
                    tr("There is no DROID SD card mounted.\n\nIf it has been "
                       "ejected, remove and reinsert it. Note: it must be a "
                       "microSD card that has already been used in your DROID master."),
                    QMessageBox::Ok);
        return;
    }

    QString compressed = patch->toDeployString();
    if (compressed.size() > MAX_DROID_INI) {
        QMessageBox::critical(
                    mainWindow,
                    tr("Your patch is too big"),
                    tr("Your patch is %1 bytes big. There is a maximum "
                       "of %2 bytes that the Droid can handle. Please check the compression "
                       "options in the preferences. ").arg(compressed.size()).arg(MAX_DROID_INI),
                    QMessageBox::Ok);
        return;
    }


    QDir dir(dirPath);
    QFileInfo droidIni(dir, DROID_PATCH_FILENAME);
    if (!patch->saveContentsToFile(droidIni.absoluteFilePath(), compressed))
    {
        QMessageBox::critical(
                    mainWindow,
                    tr("Cannot write %1").arg(QString(DROID_PATCH_FILENAME)),
                    tr("Error writing %1").arg(droidIni.absoluteFilePath()),
                    QMessageBox::Ok);
        return;
    }

    QSettings settings;
    bool ok;
    if (settings.value("activation/ship_firmware", false).toBool())
        ok = shipFirmware(dirPath);
    else
        ok = false;

    ejectSDCard(dirPath);

    if (ok) {
        HintDialog::hint("shipped_firmware",
                         tr("I have copied the firmware files for an upgrade of\n"
                            "your MASTER and MASTER18 to version %1\n"
                            "to the SD card, because you have enabled this in\n"
                            "the Preferences.\n\n"
                            "When this SD card is inserted, your master will automatically\n"
                            "upgrade to that version the next time you load the patch or power\n"
                            "up the module.\n\n"
                            "If you master already is on %1, it detects this\n"
                            "and skips the upgrade.").
                         arg(the_firmware->version()));
    }
}

void PatchOperator::ejectSDCard(QString dirPath)
{
    QDir dir(dirPath);

#ifdef Q_OS_WIN
    if (ejectSDWindows(dir.absolutePath().mid(0, 1)))
    {
        sdCardPresent = false;
        statusDumpPresent = false;
        emit droidStateChanged();
    }
    else {
        QMessageBox::warning(
                    mainWindow,
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
                    mainWindow,
                    tr("Could not eject SD card"),
                    tr("Timeout while trying to safely eject the SD card."),
                    QMessageBox::Ok);
    }
    else if (process.exitStatus() != QProcess::NormalExit) { // neither happened
        QString error = process.readAll(); // always empty
        QMessageBox::warning(
                    mainWindow,
                    tr("Could not eject SD card"),
                    tr("An error occurred while ejecting the SD card:\n%1").arg(error),
                    QMessageBox::Ok);
    }
    else if (dir.exists()) {
        QMessageBox::warning(
                    mainWindow,
                    tr("Could not eject SD card"),
                    tr("I could not eject the SD card.\n\n"
                       "Probably the card is in use by some other application. "
                       "Better go and check and eject the card with Finder before "
                       "you remove it from the card reader."),
                    QMessageBox::Ok);
    }
    else {
        // QString all = process.readAll();
        sdCardPresent = false;
        statusDumpPresent = false;
        emit droidStateChanged();
    }
#endif
}
void PatchOperator::upgradeMasterFirmware()
{
    WaitForSDDialog dialog(
        tr("DROID master firmware upgrade"),
        tr("You are going to upgrade the firmware of your MASTER or\n"
           "MASTER18 module to %1.\n\n"
           "Please insert the Droid SD card.\n").arg(the_firmware->version()),
        mainWindow,
        this);

    QString sddir = dialog.waitForSD();
    if (sddir == "")
        return; // Aborted by user

    bool ok = shipFirmware(sddir);
    ejectSDCard(sddir);

    if (ok) {
        QMessageBox::information(
            mainWindow,
            tr("Success"),
            tr("I have copied the firmware file to your SD card.\n\n"
               "Now put the SD card into your master module and press "
               "the button to start the upgrade.\n\n"
               "Hint: pressing the button only works if there is a valid "
               "Droid patch on the card. If you want to upgrade without a "
               "patch, power off and on the master while the card is inserted."));
    }
}
bool PatchOperator::shipFirmware(QString sddir)
{
    QString version = the_firmware->version();

    return copyFile(QString(":firmware/droid-%1.fw").arg(version),  sddir + "/droid.fw")
        && copyFile(QString(":firmware/master18-%1.fw").arg(version),  sddir + "/master18.fw");

}
bool PatchOperator::copyFile(QString src, QString dest)
{
    if (QFile::exists(dest)) {
        if (!QFile::remove(dest)) {
            QMessageBox::critical(
                mainWindow,
                tr("Error"),
                tr("Failed to remove in-the-way file %1:\n").arg(dest));
            return false;
        }
    }

    QFile srcFile(src);
    if (srcFile.copy(dest))
        return true;
    else {
        QMessageBox::critical(
            mainWindow,
            tr("Error"),
            tr("Could not copy the file %1 to %2:\n"
               "%3").arg(src).arg(dest).arg(srcFile.errorString()));
        return false;
    }
}
void PatchOperator::createStatusDumpsMenu()
{
     dumpsMenu = new QMenu(tr("Show status dump"));
     mainWindow->addStatusDumpsMenu(dumpsMenu);
}
void PatchOperator::loadStatusDumps()
{
    mainWindow->showStatusDump(0); // clear current status dump (avoids crash)
    statusDumps.clear();

    for (int nr=1; nr<MAX_DUMP_FILE_NUMBER; nr++) {
        QFileInfo statusFile = QFileInfo(sdCardDir(), QString(STATUS_DUMP_FILENAME).arg(nr));
        if (statusFile.isFile()) {
            QString path = statusFile.absoluteFilePath();
            try {
                statusDumps.append(StatusDump(path));
            }
            catch (ParseException &e) {
                QMessageBox::critical(
                            mainWindow,
                            tr("Could not load status dump"),
                            tr("An error occurred while loading the status dump file %1:\n\n%2")
                            .arg(path).arg(e.toString()),
                            QMessageBox::Ok);
                break;
            }
        }
        else
            break;
    }
    // Create mapping table for cable compression if that is enabled
    QSettings settings;
    bool renameCables = settings.value("compression/rename_cables", false).toBool();
    if (renameCables) {
        Patch *cloned = patch->clone();
        QMap<QString, QString> mapping;
        cloned->compressCables(&mapping);
        delete cloned;
        for (auto &dump: statusDumps) {
            dump.addCableMapping(mapping);
        }
    }

    showStatusDumpNr(statusDumps.count() - 1);
}
void PatchOperator::showStatusDumpNr(int nr)
{
    if (nr >= 0 && nr < statusDumps.count()) {
        mainWindow->showStatusDump(&statusDumps[nr]);
        lastShownStatusDumpNr = nr;
    }
    else
        mainWindow->showStatusDump(0);
    updateStatusDumpsMenu(false);
}
QString PatchOperator::sdCardDirSansPolling()
{
    QString path = savedSDCardDir();
    // check if there is a path in settings
    if (path != "") {
        // now check if path is valid
        if (isDroidVolume(path)) {
            return path;
        }
    }
    else {
        // if no path is set in settings, look for it on all attached drives
        foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes())
        {
            if (storage.fileSystemType() != "msdos" &&
                storage.fileSystemType() != "FAT32" &&
                storage.fileSystemType() != "FAT16" &&
                storage.fileSystemType() != "FAT" )
            {
                continue;
            }
            if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
                if (isDroidVolume(storage.rootPath())) {
                    // found SDCard, safe to settings and return it
                    QSettings settings;
                    settings.setValue(SD_PATH_SETTINGS_KEY_NAME, storage.rootPath());
                    return storage.rootPath();
                }
            }
        }
    }
    // path not valid atm, return ""
    return "";
}
void PatchOperator::examinePatchSize() const
{
    QSettings settings;
    unsigned compressedSize = patch->toDeployString().length();
    if (compressedSize > MAX_DROID_INI) {
        bool renameCables = settings.value("compression/rename_cables", false).toBool();
        if (!renameCables) {
            HintDialog::hint("pg_compression_off",
              tr("The patch that you have generated has a size of %1 characters.\n"
                 "This exceed the maximum patch size of %2.\n"
                 "\n"
                 "Hint: Go to the preferences and look at the box\n"\
                 "\"Compress patch before loading into master\".\n"
                 "Enable the option \"Rename patch cables\" "
                 "to reduce the patch size.\n")
                 .arg(compressedSize)
                 .arg(MAX_DROID_INI));
        }
    }

    unsigned memoryAvailable = the_firmware->availableMemory(patch->typeOfMaster());
    QStringList breakdown;
    unsigned memoryNeeded = patch->usedRAM(breakdown);
    if (memoryNeeded > memoryAvailable) {
        QString text =
            tr("Your generated patch needs too much memory. \n"
               "It needs %1 bytes, but your master only has %2,\n"
               "so that's %3 bytes too much.\n")
                           .arg(niceBytes(memoryNeeded))
                           .arg(niceBytes(memoryAvailable))
                           .arg(niceBytes(memoryNeeded - memoryAvailable));
        QSettings settings;
        if  (!settings.value("compression/deduplicate_jacks", false).toBool()) {
             text += tr("\nTry the setting \"Detect and share duplicate values \n"
                        "for inputs\" in the preferences.\"\n");
        }
        else
            text += tr("\nTry reducing the patch size by removing a feature\n"
                            "or by choosing a smaller configuration.\n");
        HintDialog::hint("pg_exceeds_ram", text);
    }

}
QString PatchOperator::sdCardDir()
{
    if (!pollSD()) {
        return sdCardDirSansPolling();
    }

    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes())
    {
         // Make sure that we avoid access to network and cloud filesystems and similar
         // stuff that can make trouble. Note: The lower case filesystem names are
         // for MAC. The upper case ones for Windows. We have seen "FAT32", but "FAT16"
         // or "FAT" is just a guess. I don't know where to get an authoritive list
         // of these identifiers.
         if (storage.fileSystemType() != "msdos" &&
            storage.fileSystemType() != "FAT32" &&
            storage.fileSystemType() != "FAT16" &&
            storage.fileSystemType() != "FAT" )
                    continue;

        if (storage.isValid() && storage.isReady() && !storage.isReadOnly()) {
            if (isDroidVolume(storage.rootPath())) {
                return storage.rootPath();
            }
        }
    }
    return "";
}
bool PatchOperator::bringToFrontIfOpen(const QString &filePath, bool inOthers)
{
    MainWindow *otherWindow = the_windowlist->windowWithFile(filePath);
    if (otherWindow && (otherWindow != mainWindow || !inOthers)) {
        otherWindow->bringToFront();
        return true;
    }
    else
        return false;

}
void PatchOperator::updateStatusDumpsMenu(bool newDumpAvailable)
{
    // The shortcut Shift+F10 is always at one of the possible actions
    // (if any of them is available):
    // 1. Load status dumps from SD card (and show the last one)
    // 2. Hide status dump
    // 3. Show again last shown status dump
    static auto shortcut = QKeySequence(tr("Shift+F10"));

    dumpsMenu->clear();
    actionLoadDumps = new QAction(
                statusDumps.empty()
                  ? tr("Load status dumps from DROID microSD card")
                  : tr("Reload status dumps"),
                this);
    actionLoadDumps->setEnabled(false);
    connect(actionLoadDumps, &QAction::triggered, this, &PatchOperator::loadStatusDumps);
    dumpsMenu->addAction(actionLoadDumps);

    // The shortcut is set as follows (in order of priority)
    // - If no status dump is loaded, the shortcut is set on load
    // - If a (new) dump becomes available on the SD card, the
    //   shortcut is also set on load
    // - If a dump is shown, the shortcut is set on hide
    // - Otherwise the shortcut is set on show

    bool shortcut_set = false;

    if (statusDumpPresent) {
        actionLoadDumps->setEnabled(true);
        if (newDumpAvailable || statusDumps.empty()) {
            actionLoadDumps->setShortcut(shortcut);
            shortcut_set = true;
        }
    }
    if (!statusDumps.empty()) {
        QAction *actionHide = new QAction(tr("Hide status dump"), this);
        connect(actionHide, &QAction::triggered, this, [this]() { this->showStatusDumpNr(-1); });
        if (mainWindow->statusDump()) {
            actionHide->setEnabled(true);
            if (!shortcut_set) {
                actionHide->setShortcut(shortcut); // Shift+F10 hides if one is shown
                shortcut_set = true;
            }
        }
        else
            actionHide->setEnabled(false);
        dumpsMenu->addAction(actionHide);

        dumpsMenu->addSeparator();

        for (int i=0; i<statusDumps.count(); i++)
        {
            QAction *action = new QAction(statusDumps[i].title(), this);
            action->setCheckable(true);
            connect(action, &QAction::triggered, this, [this, i]() { this->showStatusDumpNr(i); });
            dumpsMenu->addAction(action);
            if (mainWindow->statusDump() && lastShownStatusDumpNr == i)
                action->setChecked(true);

            // No status dump shown right now? Attach short cut to
            // either most previously shown or last dump in the list
            if (!mainWindow->statusDump()) {
                if (lastShownStatusDumpNr == i && !shortcut_set)
                    action->setShortcut(shortcut);
            }
        }
    }
}
bool PatchOperator::isDroidVolume(const QString &rootPath) const
{
    QDir dir(rootPath);
    return dir.exists("DROIDCAL.BIN") || dir.exists("DROIDSTA.BIN");
}
bool PatchOperator::pollSD() const {
    QSettings settings;
    return settings.value(POLL_SD_SETTINGS_KEY_NAME, SETTING_POLL_DEFAULT).toBool();
}
bool PatchOperator::pollX7() const {
    QSettings settings;
    return settings.value(POLL_X7_SETTINGS_KEY_NAME, SETTING_POLL_DEFAULT).toBool();
}
QString PatchOperator::savedSDCardDir() const {
    QSettings settings;
    return settings.value(SD_PATH_SETTINGS_KEY_NAME, "").toString();
}
void PatchOperator::updateSDAndX7State()
{
    bool oldSDState = sdCardPresent;
    bool oldStatusState = statusDumpPresent;

    if (pollSD())
        sdCardPresent = sdCardDir() != "";
    else
        sdCardPresent = true;

    if (sdCardPresent) {
        QFileInfo statusFile = QFileInfo(sdCardDir(), QString(STATUS_DUMP_FILENAME).arg(1));
        statusDumpPresent = statusFile.isFile() && statusFile.exists();
    }
    else
        statusDumpPresent = false;

    if (oldSDState != sdCardPresent || oldStatusState != statusDumpPresent) {
        if (oldStatusState != statusDumpPresent)
            updateStatusDumpsMenu(statusDumpPresent);
        emit droidStateChanged();
        return;
    }

    bool oldX7State = x7Present;
    if (pollX7())
        x7Present = midiHost.x7Connected();
    else
        x7Present = true;

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
                        mainWindow,
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
                                mainWindow,
                                tr("Cannot restore backup"),
                                tr("Parse error in the backup file. The backup cannot be restored. I will use the original file."),
                                QMessageBox::Ok);
                }
            }
        }
        else
            integratePatch(filePath);
        emit patchModified();
        the_windowlist->update();
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
void PatchOperator::openFileFromExternal(const QString &filePath)
{
    if (!bringToFrontIfOpen(filePath, true))
        loadFile(filePath, FILE_MODE_LOAD);
}
void PatchOperator::open()
{
    QString filePath = QFileDialog::getOpenFileName(
                mainWindow, "", "", "DROID patches (*.ini)");

    if (!filePath.isEmpty() && !bringToFrontIfOpen(filePath, true))
        loadFile(filePath, FILE_MODE_LOAD);
}
void PatchOperator::openRecentFile(const QString filePath)
{
    if (!filePath.isEmpty() && !bringToFrontIfOpen(filePath, true))
        loadFile(filePath, FILE_MODE_LOAD);
}
void PatchOperator::openInNewWindow()
{
    QString filePath = QFileDialog::getOpenFileName(
                mainWindow, "", "", "DROID patches (*.ini)");

    if (!filePath.isEmpty() && !bringToFrontIfOpen(filePath, false)) {
        MainWindow *newWindow = new MainWindow(filePath);
        newWindow->show();
    }
}
void PatchOperator::newWindow()
{
    MainWindow *newWindow = new MainWindow("");
    newWindow->show();
}
void PatchOperator::newWindowWithSameRack()
{
    MainWindow *newWindow = new MainWindow("", patch);
    newWindow->show();
}
void PatchOperator::clearRecentFiles()
{
    QSettings settings;
    QStringList emptyList;
    settings.setValue("recentfiles", emptyList);
    createRecentFileActions(recentFilesMenu);
}
void PatchOperator::search(QString text, int direction)
{
    int startSectionIndex = patch->currentSectionIndex();
    CursorPosition startPos = patch->currentSection()->cursorPosition();
    bool found = false;

    while (true) {
        if (direction == 1)
            patch->moveCursorForward(false /* no auto unfold */);
        else
            patch->moveCursorBackward(false /* no auto unfold */);
        int nextSectionIndex = patch->currentSectionIndex();
        CursorPosition nextPos = patch->currentSection()->cursorPosition();
        if (nextSectionIndex == startSectionIndex && nextPos == startPos)
            break;

        else if (patch->currentSection()->searchHitAtCursor(text)) {
            found = true;
            if (patch->currentSectionIndex() != startSectionIndex)
                emit sectionSwitched();
            else
                emit cursorMoved();
            break;
        }
    }

    // First round: We count the number of search hits
    unsigned pos, count;
    pos = patch->searchHitPosition(text, &count);
    if (found) emit patchModified(); // circuit might have been unfolded. We need a repaint.
    emit searchStatsChanged(pos, count);
}

void PatchOperator::openPatchGenerator(int index, PatchGenerator *gen)
{
    if (!checkModified())
        return;

    mainWindow->updateGeneratorsShortcut(index);

    Patch *newPatch = PatchGeneratorDialog::generatePatch(gen);
    if (!newPatch)
        return;

    // Move contents of new patch into "our" patch without
    // invalidating its pointer
    patch->startFromScratch();
    patch->setFilePath("");
    newPatch->cloneInto(patch);

    // Brauchen wir das hier wirklich??
    // Beware: a patch *must* have at least one section. If we load
    // an empty patch, we crash if there is no section
    // if (patch->numSections() == 0)
    //     patch->addSection(new PatchSection());

    patch->commit(tr("generating patch"));
    setLastFilePath("");
    emit patchModified();

    examinePatchSize();

}
void PatchOperator::integrate()
{
    QString filePath = QFileDialog::getOpenFileName(mainWindow);
    if (!filePath.isEmpty())
        loadFile(filePath, FILE_MODE_INTEGRATE);
}
void PatchOperator::exportSelection()
{
    QString filePath = QFileDialog::getSaveFileName(
                mainWindow,
                tr("Export selection into new patch"),
                "",
                tr("DROID patch files (*.ini)"));
    if (!filePath.isEmpty()) {
        Patch *patch = section()->getSelectionAsPatch();
        if (!patch->saveContentsToFile(filePath, patch->toString()))
        {
            QMessageBox::warning(
                        mainWindow,
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
    QString oldBackupPath = backupFilePath(patch->getFilePath());

    QString newFilePath = QFileDialog::getSaveFileName(
                mainWindow,
                tr("Save patch to new file"),
                patch->getFilePath(),
                tr("DROID patch files (*.ini)"));
    if (!newFilePath.isEmpty()) {
        MainWindow *otherWindow = the_windowlist->windowWithFile(newFilePath);
        if (otherWindow && otherWindow != mainWindow) {
            QMessageBox::warning(
                        mainWindow,
                        tr("Error"),
                        tr("This file is already open in another window."));
            otherWindow->bringToFront();
            return;
        }
        if (saveAndCheck(newFilePath)) {
            addToRecentFiles(newFilePath);
            QFile file(oldBackupPath);
            if (file.exists())
                file.remove();
        }
        the_windowlist->update();
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
                    mainWindow,
                    tr("Error"),
                    tr("There was an error saving your patch to disk"));
    return false;
}
QString PatchOperator::backupFilePath(QString path)
{
    if (path != "") // File has already a name
        return path + ".save";
    else {
        if (!untitledBackupNumber)
            untitledBackupNumber = nextFreeUntitledBackup();
        return untitledBackupPath(untitledBackupNumber);
    }
}
QString PatchOperator::untitledBackupPath(unsigned int number)
{
    return QString(UNTITLED_BACKUP_FILENAME).arg(number);

}
unsigned int PatchOperator::nextFreeUntitledBackup()
{
    for (unsigned n=1; n < MAX_UNTITLED_BACKUPS; n++) {
        QString path = untitledBackupPath(n);
        QFile file(path);
        if (!file.exists())
            return n;
    }
    return 0;
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
    patch->saveContentsToFile(backupname, patch->toString());
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
                    mainWindow,
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
    otherPatch->collectUsedRegisterAtoms(neededRegisters, true /* skip overlayed controls */);

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
                    mainWindow,
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
                            mainWindow,
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

    bool yesToAll = false;
    bool noToAll = false;

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
                    int reply;
                    if (yesToAll)
                        reply = QMessageBox::Yes;
                    else if (noToAll)
                        reply = QMessageBox::No;
                    else
                        reply = QMessageBox::question(
                                mainWindow,
                                tr("Cable conflict"),
                                tr("The internal patch cable \"%1\" is used as output both by "
                                   "the existing and the integrated patch. This will result in a "
                                   "problem.\n\nShall I rename that cable in order to avoid fear, "
                                   "uncertainty and doubt?").arg(cable),
                                QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No |
                                QMessageBox::YesToAll | QMessageBox::NoToAll,
                                QMessageBox::Yes);

                    if (reply == QMessageBox::YesToAll) {
                        yesToAll = true;
                        reply = QMessageBox::Yes;
                    }
                    else if (reply == QMessageBox::NoToAll) {
                        noToAll = true;
                        reply = QMessageBox::No;
                    }

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
    Patch *parsed = editSource(title, section()->toString(true /* supress empty header */));
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
    // Add one empty line, otherwise we will lose the first comment
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
    showSource(tr("Patch source without comments"), patch->toBareString());
}
void PatchOperator::exportCompressedPatch()
{
    HintDialog::hint("export_compressed_patch",
                     tr("When you deploy a patch to your master with \"Activate\"\n"
                        "or \"Save to SD\", the patch source code is optimized\n"
                        "and compressed in various ways, to make it smaller and use\n"
                        "less RAM. There are various options in the Preferences for\n"
                        "tweaking this.\n"
                        "\n"
                        "You are going to export such a compressed version of your\n"
                        "patch to a file. There you can examine it or copy it manually\n"
                        "to a SD card for a Droid master.\n"
                        "\n"
                        "Note: This is not the same as saving a patch. The exported\n"
                        "patch does not contain comments or section titles. Maybe patch\n"
                        "cable names are replaced by short abbreviations, etc.\n"
                        "Depending on your settings the exported file may not even\n"
                        "be loadable by the Forge later. It is only meant for deploying\n"
                        "a patch on a master.\n"
                        ));

    QString exportPath = QFileDialog::getSaveFileName(
                mainWindow,
                tr("Export compressed patch"),
                "",
                tr("DROID patch files (*.ini)"));

    if (exportPath == "")
        return;

    QFile file(exportPath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(
            0,
            TR("File cannot be opened"),
            TR("Exporting the patch to the file\n\n"
               "%1\n\n"
               "does not work. I cannot create that file.").arg(exportPath),
            QMessageBox::Ok);
        return;
    }

    QTextStream stream(&file);
    stream << patch->toDeployString();
    stream.flush();
    file.close();
    if (stream.status() != QTextStream::Ok) {
        QMessageBox::critical(
            0,
            tr("Write error"),
            tr("There was an error while writing the contents to the file"),
            QMessageBox::Ok);
    }
}
void PatchOperator::patchMemoryAnalysis()
{
    MemoryAnalysisWindow maw(patch, mainWindow);
    maw.exec();
}
void PatchOperator::fixLEDMismatch()
{
    section()->currentCircuit()->fixLEDMismatches();
    section()->sanitizeCursor();
    patch->commit(tr("fixing LED mismatches"));
    emit patchModified();
}
void PatchOperator::rewriteCableNames()
{
    static RewriteCablesDialog *dialog = 0;

    // Dialog aufmachen, der search + replace erfragt
    QString search;
    QString replace;

    if (!dialog)
        dialog = new RewriteCablesDialog();

    while (true) {
        if (dialog->exec() == QDialog::Accepted)
        {
            QString error = dialog->validateInput();
            if (error != "") {
                QMessageBox::critical(
                            mainWindow,
                            tr("Invalid input, please try again"),
                            error,
                            QMessageBox::Ok);
                continue;
            }
            patch->rewriteCableNames(dialog->getRemoved(), dialog->getInserted(), dialog->getMode());
            patch->commit(tr("Rewriting patch cable names"));
            emit patchModified();
            return;
        }
        else {
            return;
        }
    }
}
void PatchOperator::setBookmark()
{
    patch->clearBookmarks();
    patch->setBookmark();
    mainWindow->setStatusbarText(tr("Bookmark set"));
    emit patchModified();
}
void PatchOperator::jumpToBookmark()
{
    int sectionNr;
    CursorPosition pos;
    bool found;
    found = patch->findBookmark(&sectionNr, &pos);
    if (!found)
        return;

    patch->switchCurrentSection(sectionNr);
    patch->setCursorTo(sectionNr, pos);
    emit sectionSwitched();
}
void PatchOperator::globalClipboardChanged()
{
    the_clipboard->copyFromGlobalClipboard();
}
void PatchOperator::modifyPatch()
{
    if (patch->isModified())
        createBackup();
    else
        removeBackup();
}
void PatchOperator::close()
{
    if (checkModified())
        mainWindow->close();
}
Patch *PatchOperator::editSource(const QString &title, QString oldSource)
{
    SourceCodeEditor editor(title, oldSource, mainWindow, false /* readonly */);
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
                        mainWindow,
                        tr("Parse error"),
                        e.toString(),
                        QMessageBox::Ok);
        }
    }
}
void PatchOperator::showSource(const QString &title, QString source)
{
    SourceCodeEditor editor(title, source, mainWindow, true /* readonly */);
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
