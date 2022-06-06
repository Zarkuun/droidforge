#ifndef PATCHOPERATOR_H
#define PATCHOPERATOR_H

#include "patcheditengine.h"
#include "editoractions.h"
#include "patchparser.h"

#include <QObject>
#include <QFileInfo>


class PatchOperator : public QObject
{
    Q_OBJECT
    PatchEditEngine *patch;
    PatchParser parser;
    bool sdCardPresent;

public:
    explicit PatchOperator(PatchEditEngine *patch, QString initialFilename);
    void createRecentFileActions(QMenu *);
    void loadFile(const QString &filename, int how);
    void quit();
    void jumpTo(int sectionIndex, const CursorPosition &pos);
    void clearSelection();
    bool droidSDCardPresent() const { return sdCardPresent; };

protected:
    PatchSection *section() { return patch->currentSection(); };
    const PatchSection *section() const { return patch->currentSection(); };

private slots:
    void upload();
    void saveToSD();
    void newPatch();
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

private:
    bool checkModified();
    void setLastFilePath(const QString &path);
    void openDirInFinder(const QString &filename);
    void addToRecentFiles(const QString &path);
    QStringList getRecentFiles();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    bool interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch=0);
    bool isDroidVolume(const QFileInfo &fileinfo) const;
    void updateSDState();
    QDir sdCardDir() const;

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
