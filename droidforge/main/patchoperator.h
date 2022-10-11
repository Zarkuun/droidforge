#ifndef PATCHOPERATOR_H
#define PATCHOPERATOR_H

#include "patcheditengine.h"
#include "editoractions.h"
#include "patchparser.h"
#include "os.h"

#ifdef Q_OS_MAC
#include "macmidihost.h"
#define OurMIDIHost MacMIDIHost
#else
#include "windowsmidihost.h"
#define OurMIDIHost WindowsMIDIHost
#endif

#include <QObject>
#include <QFileInfo>


class PatchOperator : public QObject
{
    Q_OBJECT
    PatchEditEngine *patch;
    PatchParser parser;
    bool sdCardPresent;
    bool x7Present;
    OurMIDIHost midiHost;
    QMenu *recentFilesMenu;


public:
    explicit PatchOperator(PatchEditEngine *patch, QString initialFilename);
    void createRecentFileActions(QMenu *menu);
    void loadFile(const QString &filename, int how);
    void quit();
    void jumpTo(int sectionIndex, const CursorPosition &pos);
    void clearSelection();
    bool droidSDCardPresent() const { return sdCardPresent; };
    bool droidX7Present() const { return x7Present; };
    bool interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch=0);
    bool isPatching() const { return patch->isPatching(); };
    bool isPatchingFrom(const CursorPosition &pos) const;

protected:
    PatchSection *section() { return patch->currentSection(); };
    const PatchSection *section() const { return patch->currentSection(); };

private slots:
    void abortAllActions();
    void upload();
    void saveToSD();
    void newPatch();
    void newPatchWithSameRack();
    void open();
    void integrate();
    void exportSelection();
    void save();
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
    void globalClipboardChanged();
    void modifyPatch();

public slots:
    void clearRecentFiles();

private:
    bool checkModified();
    void setLastFilePath(const QString &path);
    void openDirInFinder(const QString &filename);
    void addToRecentFiles(const QString &path);
    QStringList getRecentFiles();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    bool isDroidVolume(const QString &rootPath) const;
    void updateSDAndX7State();
    QString sdCardDir() const;
    Patch *editSource(const QString &title, QString oldSource);
    void showSource(const QString &title, QString source);
    bool saveAndCheck(QString path);
    QString backupFilePath(QString path);
    void restoreBackup(const QString &backupPath);
    void createBackup();
    void removeBackup();
#ifdef Q_OS_WIN
    bool ejectSDWindows(const QString letter);
#endif


signals:
    void patchModified();
    void clipboardChanged();
    void selectionChanged();
    void sectionSwitched();
    void cursorMoved();
    void patchingChanged();
    void droidStateChanged();
};

extern PatchOperator *the_operator;

#endif // PATCHOPERATOR_H
