#ifndef UPDATEHUB_H
#define UPDATEHUB_H

#include <QObject>

class UpdateHub : public QObject
{
    Q_OBJECT
public:
    explicit UpdateHub(QObject *parent = nullptr);

public slots:
    void changePatch();
    void switchSection();

signals:
    void patchChanged();
    void sectionSwitched();
};

extern UpdateHub *the_hub;

#endif // UPDATEHUB_H
