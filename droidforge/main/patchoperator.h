#ifndef PATCHOPERATOR_H
#define PATCHOPERATOR_H

#include "patcheditengine.h"
#include "editoractions.h"

#include <QObject>


class PatchOperator : public QObject
{
    Q_OBJECT
    PatchEditEngine *patch;

public:
    explicit PatchOperator(PatchEditEngine *patch);

private slots:
    void undo();
    void redo();

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
