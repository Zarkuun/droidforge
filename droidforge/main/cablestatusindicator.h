#ifndef CABLESTATUSINDICATOR_H
#define CABLESTATUSINDICATOR_H

#include "versionedpatch.h"

#include <QPropertyAnimation>
#include <QWidget>
#include <QPen>
#include <QPainter>

class CableStatusIndicator : public QWidget
{
    Q_OBJECT
    const VersionedPatch *patch;
    QPen cablePen;
    QPen cableHilitePen;
    QImage warningImage;

    float animationPhase; // TODO: Do I need this?
    QPropertyAnimation animation;

    // Current state to be displayed
    QString cableName;
    int numAsInput;
    int numAsOutput;
    bool patching;

public:
    explicit CableStatusIndicator(const VersionedPatch *patch, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);

    // TODO: Das set/clear besser mit Slot/Signal machen.
    // PatchView oder SectionView->signal atomchanged...
    // Das hier verbinden
    void set(QString name, int numAsInput, int numAsOutput);
    void clear();
    void setPatchingState(bool);
    float getanimationPhase() const { return animationPhase; };
    void setanimationPhase(float newanimationPhase);

private:
    void paintPatching(QPainter &painter);
    void paintCableInfo(QPainter &painter);
    void paintCable(QPainter &painter, int left, int right);
    void paintMarker(QPainter &painter, int xpos, QColor border, QColor fill, int number);
    void paintLabel(QPainter &painter, int xpos, QString text);

signals:
    void animationPhaseChanged();

private slots:
    void patchChanged(VersionedPatch *patch);
    void updateStatus();

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
};

#endif // CABLESTATUSINDICATOR_H
