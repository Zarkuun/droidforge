#ifndef UPDATEHUB_H
#define UPDATEHUB_H

#include <QObject>

class VersionedPatch;

class UpdateHub : public QObject
{
    Q_OBJECT
public:
    explicit UpdateHub(QObject *parent = nullptr);

public slots:
    void changePatch(VersionedPatch *patch);
    void modifyPatch();
    void switchSection();
    void changeClipboard();
    // TODO: Esc -> Abort all actions

signals:
    void patchChanged(VersionedPatch *patch);
    void patchModified();
    void sectionSwitched();
    void clipboardChanged();
};

extern UpdateHub *the_hub;

#endif // UPDATEHUB_H
