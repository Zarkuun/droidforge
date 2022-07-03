#include "animatedindicator.h"

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
