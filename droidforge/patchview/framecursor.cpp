#include "framecursor.h"
#include "tuning.h"

#include <QPen>

FrameCursor::FrameCursor()
    : animation(this, "animationPhase")
{
    setZValue(50);

    animation.setDuration(800);
    animation.setKeyValueAt(0, 0.0);
    animation.setKeyValueAt(0.1, 1.0);
    animation.setKeyValueAt(1, 0.0);
    animation.setEasingCurve(QEasingCurve::OutQuad);
    // animation->setTargetObject(this);
}

FrameCursor::~FrameCursor()
{
}

float FrameCursor::getanimationPhase() const
{
    return animationPhase;
}

void FrameCursor::setanimationPhase(float newAnimationPhase)
{
    // if (qFuzzyCompare(animationPhase, newAnimationPhase))
    //     return;
    // Braucht man das vielleicht doch?
    animationPhase = newAnimationPhase;

    QPen pen;
    pen.setWidth(1);
    pen.setColor(COLOR_FRAME_CURSOR);
    setPen(pen);

    QColor c = COLOR_FRAME_CURSOR_BACKGROUND;
    c.setAlphaF(animationPhase * 0.3);
    setBrush(c); // QColor(255, 60, 10, 50));

    emit animationPhaseChanged();
}

void FrameCursor::startAnimation()
{
    setanimationPhase(0.0);
    animation.setCurrentTime(0);
    animation.start();
}
