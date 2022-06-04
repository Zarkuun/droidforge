#ifndef PATCHOPERATOR_H
#define PATCHOPERATOR_H

#include "patcheditengine.h"
#include "editoractions.h"
#include "patchparser.h"

#include <QObject>


class PatchOperator : public QObject
{
    Q_OBJECT
    PatchEditEngine *patch;
    PatchParser parser;

public:
    explicit PatchOperator(PatchEditEngine *patch);
    void createRecentFileActions(QMenu *);
    void loadFile(const QString &filename, int how);
    bool checkModified();

protected:
    PatchSection *section() { return patch->currentSection(); };
    const PatchSection *section() const { return patch->currentSection(); };

private slots:
    void newPatch();
    void open();
    void integrate();
    void exportSelection();
    void save();
    void saveAs();
    void editProperties();
    void undo();
    void redo();

private:
    void setLastFilePath(const QString &path);
    void addToRecentFiles(const QString &path);
    QStringList getRecentFiles();
    void loadPatch(const QString &aFilePath);
    void integratePatch(const QString &aFilePath);
    bool interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch=0);

signals:
    void patchModified();
    void clipboardChanged();
    void selectionChanged();
    void sectionSwitched();
    void cursorMoved();
    void patchingChanged();
};

extern PatchOperator *the_operator;

#endif // PATCHOPERATOR_H
