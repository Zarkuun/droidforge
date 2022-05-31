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
    void switchSection();

signals:
    void patchChanged(VersionedPatch *patch);
    void sectionSwitched();
};

extern UpdateHub *the_hub;

#endif // UPDATEHUB_H
