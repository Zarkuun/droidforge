#ifndef CABLESTATUSINDICATOR_H
#define CABLESTATUSINDICATOR_H

#include "patcheditengine.h"
#include "patchview.h"

#include <QPropertyAnimation>
#include <QWidget>
#include <QPen>
#include <QPainter>

class CableStatusIndicator : public QWidget, PatchView
{
    Q_OBJECT
    QPen cablePen;
    QPen cableHilitePen;
    QImage warningImage;

    float animationPhase;
    QPropertyAnimation animation;

    // Current state to be displayed
    QString cableName;
    int numAsInput;
    int numAsOutput;

public:
    explicit CableStatusIndicator(PatchEditEngine *patch, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void set(QString name, int numAsInput, int numAsOutput);
    void clear();
    void updatePatchingAnimation();
    float getanimationPhase() const { return animationPhase; };
    void setanimationPhase(float newanimationPhase);

private:
    void paintPatching(QPainter &painter);
    void paintCableInfo(QPainter &painter);
    void paintCable(QPainter &painter, int left, int right);
    void paintMarker(QPainter &painter, int xpos, QColor border, QColor fill, int number);
    void paintLabel(QPainter &painter, int xpos, QString text);

signals:
    void clicked();
    void animationPhaseChanged();

private slots:
    void updateStatus();
    void changePatching();

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
};

#endif // CABLESTATUSINDICATOR_H
