#ifndef REGISTERMARKER_H
#define REGISTERMARKER_H

#include <QGraphicsEllipseItem>
#include <QPropertyAnimation>
#include <QObject>

class RegisterMarker : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

    float animationPhase; // TODO: Do I need this?
    QPropertyAnimation animation;
    QPointF center;
    float diameter;

public:
    RegisterMarker();
    float getanimationPhase() const;
    void setanimationPhase(float newanimationPhase);
    void startAnimation();
    void setCenter(QPointF c) { center = c; };
    void setDiameter(float d) { diameter = d; };

signals:
    void animationPhaseChanged();

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
};

#endif // REGISTERMARKER_H
