#include "framecursor.h"
#include "tuning.h"
#include "colorscheme.h"
#include "globals.h"

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
}

FrameCursor::~FrameCursor()
{
}

void FrameCursor::setMode(cursor_mode_t m)
{
   if (m == CURSOR_DISABLED)
       setPen(COLOR(COLOR_CURSOR_DISABLED));
   else if (m == CURSOR_PATCHING)
       setPen(COLOR(COLOR_CURSOR_PATCHING));
   else if (m == CURSOR_PROBLEM)
       setPen(COLOR(COLOR_CURSOR_PROBLEM));
   else
       setPen(COLOR(COLOR_CURSOR_NORMAL));
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
    setBrush(c);

    emit animationPhaseChanged();
}

void FrameCursor::startAnimation()
{
    if (lastRect != boundingRect()) {
        setanimationPhase(0.0);
        animation.setCurrentTime(0);
        animation.start();
        lastRect = boundingRect();
    }
}
