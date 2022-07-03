#ifndef ANIMATEDINDICATOR_H
#define ANIMATEDINDICATOR_H

#include <QGraphicsItem>
#include <QPropertyAnimation>
#include <QObject>

class AnimatedIndicator : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    float animationPhase; // TODO: Do I need this?
    QPropertyAnimation animation;

public:
    AnimatedIndicator();
    float getanimationPhase() const;
    void setanimationPhase(float newAnimationPhase);
    void doSuccessAnimation();
    void abortAnimation();

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)

signals:
    void animationPhaseChanged();

private slots:
    void endOfAnimation();
};

#endif // ANIMATEDINDICATOR_H
