#include "animatedindicator.h"

#include <QPainter>

AnimatedIndicator::AnimatedIndicator()
     : animation(this, "animationPhase")
{
    setZValue(100);
    animation.setDuration(300);
    animation.setKeyValueAt(0, 0.0);
    animation.setKeyValueAt(1, 1.0);
    connect(&animation, &QPropertyAnimation::finished, this, &AnimatedIndicator::endOfAnimation);

}
float AnimatedIndicator::getanimationPhase() const
{
    return animationPhase;
}
void AnimatedIndicator::setanimationPhase(float newAnimationPhase)
{
    int c = newAnimationPhase * 5;
    setVisible(c % 2);
    update();
}
void AnimatedIndicator::doSuccessAnimation()
{
    setanimationPhase(0.0);
    animation.setCurrentTime(0);
    animation.start();
}
void AnimatedIndicator::abortAnimation()
{
    animation.stop();
}
void AnimatedIndicator::endOfAnimation()
{
    setVisible(false);
}
void AnimatedIndicator::paintArrowHead(QPainter *painter, float size)
{
    QPolygon poly;
    poly << QPoint(0, 0)
         << QPoint(-size, -2 * size)
         << QPoint( size, -2 * size)
         << QPoint(0,0);
    painter->drawPolygon(poly);
}
