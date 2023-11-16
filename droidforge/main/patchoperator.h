#ifndef PATCHOPERATOR_H
#define PATCHOPERATOR_H

#define POLL_SD_SETTINGS_KEY_NAME       "activation/poll_for_sd"
#define POLL_X7_SETTINGS_KEY_NAME       "activation/poll_for_x7"
#define SD_PATH_SETTINGS_KEY_NAME       "activation/sd_path"

#include "patcheditengine.h"
#include "editoractions.h"
#include "patchgenerator.h"
#include "patchparser.h"

#ifdef Q_OS_MAC
#include "macmidihost.h"
#define OurMIDIHost MacMIDIHost
#else
#include "windowsmidihost.h"
#define OurMIDIHost WindowsMIDIHost
#endif

#include <QObject>
#include <QFileInfo>
#include <QMenu>
#include <QSettings>

#include "statusdump.h"

class MainWindow;


class PatchOperator : public QObject
{
    Q_OBJECT
    MainWindow *mainWindow;
    PatchEditEngine *patch;
    PatchParser parser;
    bool sdCardPresent;
    bool statusDumpPresent;
    bool x7Present;
    OurMIDIHost midiHost;
    QMenu *recentFilesMenu;
    QList<StatusDump> statusDumps;
    int lastShownStatusDumpNr;
    QMenu *dumpsMenu;
    QAction *actionLoadDumps;
    unsigned untitledBackupNumber;

public:
    explicit PatchOperator(MainWindow *mainWindow, PatchEditEngine *patch, QString initialFilename, const Patch *initialRack=0);
    void createRecentFileActions(QMenu *menu);
    QMenu *statusDumpsMenu() { return dumpsMenu; };
    void createStatusDumpsMenu();
    void loadFile(const QString &filename, int how);
    void loadUntitledBackup(const QString &filename);
    bool checkModified();
    void jumpTo(int sectionIndex, const CursorPosition &pos);
    void clearSelection();
    bool droidSDCardPresent() const { return sdCardPresent; };
    bool droidStatusDumpPresent() const { return statusDumpPresent; };
    bool droidX7Present() const { return x7Present; };
    bool interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch=0);
    bool isPatching() const { return patch->isPatching(); };
    bool isPatchingFrom(const CursorPosition &pos) const;
    void addToRecentFiles(const QString &path);
    void openFileFromExternal(const QString &filePath);
    static QString untitledBackupPath(unsigned number);

protected:
    PatchSection *section() { return patch->currentSection(); };
    const PatchSection *section() const { return patch->currentSection(); };

private slots:
    void abortAllActions();
    void upload();
    void saveToSD();
    void loadStatusDumps();
    void newPatch();
    void showStatusDumpNr(int nr);
    void newPatchWithSameRack();
    void open();
    void openRecentFile(const QString filePath);
    void openInNewWindow();
    void newWindow();
    void newWindowWithSameRack();
    void integrate();
    void exportSelection();
    void saveAs();
    void openEnclosingFolder();
    void editProperties();
    void undo();
    void redo();
    void configureColors();
    void moveCircuitUp();
    void moveCircuitDown();
    void editPatchSource();
    void editSectionSource();
    void editCircuitSource();
    void barePatchSource();
    void fixLEDMismatch();
    void rewriteCableNames();
    void setBookmark();
    void jumpToBookmark();
    void globalClipboardChanged();
    void modifyPatch();
    void close();
    void quit();
    void expandArray(bool max = false);
    void expandArrayMax();
    void renameCable();

public slots:
    void save();
    void clearRecentFiles();
    void search(QString text, int direction);
    void openPatchGenerator(PatchGenerator *gen);

private:
    void clearWithControllersFromOtherRack(const Patch *other);
    void setLastFilePath(const QString &path);
    void openDirInFinder(const QString &filename);
    QStringList getRecentFiles();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    bool isDroidVolume(const QString &rootPath) const;
    void updateSDAndX7State();
    QString sdCardDir();
    Patch *editSource(const QString &title, QString oldSource);
    void showSource(const QString &title, QString source);
    bool saveAndCheck(QString path);
    QString backupFilePath(QString path);
    unsigned nextFreeUntitledBackup();
    void restoreBackup(const QString &backupPath);
    void createBackup();
    void removeBackup();
    bool bringToFrontIfOpen(const QString &filePath, bool inOthers);
    void updateStatusDumpsMenu(bool newDumpAvailable);
    bool pollSD() const;
    bool pollX7() const;
    QString savedSDCardDir() const;
    QString sdCardDirSansPolling();

#ifdef Q_OS_WIN
    bool ejectSDWindows(const QString letter);
#endif

signals:
    void patchModified();
    void selectionChanged();
    void sectionSwitched();
    void cursorMoved();
    void patchingChanged();
    void droidStateChanged();
    void searchStatsChanged(unsigned pos, unsigned count);
};

#endif // PATCHOPERATOR_H
