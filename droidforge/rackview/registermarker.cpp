#include "registermarker.h"
#include "colorscheme.h"
#include "tuning.h"

#include <QPen>

RegisterMarker::RegisterMarker()
     : animation(this, "animationPhase")
{
    QPen pen;
    pen.setWidth(RACV_REGMARKER_PEN_WIDTH);
    pen.setColor(COLOR(RACV_REGMARKER_PEN_COLOR));
    setPen(pen);
    setBrush(COLOR(RACV_REGMARKER_BACKGROUND));
    setZValue(50);

    animation.setDuration(1000);
    animation.setKeyValueAt(0, 0.0);
    animation.setKeyValueAt(0.1, 1.0);
    animation.setKeyValueAt(1, 0.0);
    animation.setEasingCurve(QEasingCurve::OutQuad);
}

float RegisterMarker::getanimationPhase() const
{
    return animationPhase;
}

void RegisterMarker::setanimationPhase(float newAnimationPhase)
{
    animationPhase = newAnimationPhase;

    float d = diameter * (animationPhase * 0.2 + 1);
    QPointF pos = QPointF(- d/2, - d/2);
    setRect(QRectF(pos, QSize(d, d)));

    QPen p = pen();
    p.setWidth(RACV_REGMARKER_PEN_WIDTH * (1 + animationPhase));
    QColor c = COLOR(RACV_REGMARKER_PEN_COLOR);
    c.setAlphaF(animationPhase * 0.3 + 0.7);
    p.setColor(c);

    setPen(p);
    emit animationPhaseChanged();
}

void RegisterMarker::startAnimation()
{
    setanimationPhase(0.0);
    animation.setCurrentTime(0);
    animation.start();
}
