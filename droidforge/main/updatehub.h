#ifndef UPDATEHUB_H
#define UPDATEHUB_H

#include <QObject>

class PatchEditEngine;
class Selection;

class UpdateHub : public QObject
{
    Q_OBJECT
public:
    explicit UpdateHub(QObject *parent = nullptr);

public slots:
    void modifyPatch();
    void switchSection();
    void changeClipboard();
    void changeSelection();
    void moveCursor();
    void changePatching();
    void changeDroidState();
    // TODO: Esc -> Abort all actions

signals:
    void patchModified();
    void sectionSwitched();
    void clipboardChanged();
    void selectionChanged();
    void patchingChanged();
    void cursorMoved();
    void droidStateChanged();
};


// Note: There is a certain hierarchy of events. Whenever
// a "stronger" event occurs, the "weaker" dependent events
// are implied and not sent extra. If a listener ist just
// interested in the weakest event, it must subscribe to
// all higher events, nevertheless.
//
// A sender must just send the highest event in the chain
// that is sufficient for the update. So if it modified the
// patch *and* the cursor moved, just patchModified must be
// sent.

// patchModified > sectionSwitched > cursorMoved

extern UpdateHub *the_hub;

#endif // UPDATEHUB_H
