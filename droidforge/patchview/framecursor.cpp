#include "framecursor.h"
#include "editoractions.h"
#include "tuning.h"
#include "colorscheme.h"
#include "globals.h"
#include "mainwindow.h"

#include <QPen>

FrameCursor::FrameCursor(MainWindow *mainWindow)
    : mainWindow(mainWindow)
    , animation(this, "animationPhase")
{
    setMode(CURSOR_NORMAL);
    setZValue(50);

    animation.setDuration(800);
    animation.setKeyValueAt(0, 0.0);
    animation.setKeyValueAt(0.1, 1.0);
    animation.setKeyValueAt(1, 0.0);
    animation.setEasingCurve(QEasingCurve::OutQuad);
    setPen(color);
}
FrameCursor::~FrameCursor()
{
}
void FrameCursor::setMode(cursor_mode_t m)
{
   if (m == CURSOR_DISABLED)
       color = COLOR(COLOR_CURSOR_DISABLED);
   else if (m == CURSOR_PATCHING)
       color = COLOR(COLOR_CURSOR_PATCHING);
   else if (m == CURSOR_PROBLEM)
       color = COLOR(COLOR_CURSOR_PROBLEM);
   else
       color = COLOR(COLOR_CURSOR_NORMAL);
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

    if (textMode()) {
        QColor c = color;
        c.setAlphaF(0.1 + animationPhase * 0.3);
        setBrush(c);
        setPen(Qt::NoPen);
    }
    else {
        QColor c = color;
        c.setAlphaF(animationPhase * 0.3);
        setBrush(c);
        setPen(color);
    }

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
bool FrameCursor::textMode() const
{
    return ACTION(ACTION_TEXT_MODE)->isChecked();
}
