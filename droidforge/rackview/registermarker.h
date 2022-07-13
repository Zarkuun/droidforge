#ifndef REGISTERMARKER_H
#define REGISTERMARKER_H

#include <QGraphicsEllipseItem>
#include <QPropertyAnimation>
#include <QObject>

class RegisterMarker : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

    float animationPhase;
    QPropertyAnimation animation;
    float diameter;

public:
    RegisterMarker();
    float getanimationPhase() const;
    void setanimationPhase(float newAnimationPhase);
    void startAnimation();
    void setDiameter(float d) { diameter = d; };

signals:
    void animationPhaseChanged();

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
};

#endif // REGISTERMARKER_H
