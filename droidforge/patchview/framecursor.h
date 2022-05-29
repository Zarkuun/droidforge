#ifndef FRAMECURSOR_H
#define FRAMECURSOR_H

#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QObject>

class FrameCursor : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

    float animationPhase; // TODO: Do I need this?
    QPropertyAnimation animation;

public:
    FrameCursor();
    ~FrameCursor();
    float getanimationPhase() const;
    void setanimationPhase(float newanimationPhase);
    void startAnimation();

signals:
    void animationPhaseChanged(); // TODO: needed?

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
};

#endif // FRAMECURSOR_H
