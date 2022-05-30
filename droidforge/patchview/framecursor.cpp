#include "framecursor.h"
#include "tuning.h"

#include <QPen>

FrameCursor::FrameCursor()
    : animation(this, "animationPhase")
{
    setMode(CURSOR_NORMAL);
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

void FrameCursor::setMode(cursor_mode_t m)
{
   if (m == CURSOR_PATCHING)
       setPen(COLOR_CURSOR_PATCHING);
   else if (m == CURSOR_PROBLEM)
       setPen(COLOR_CURSOR_PROBLEM);
   else
       setPen(COLOR_CURSOR_NORMAL);
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

    QColor c = pen().color();
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
