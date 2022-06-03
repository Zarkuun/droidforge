#ifndef DRAGREGISTERINDICATOR_H
#define DRAGREGISTERINDICATOR_H

#include <QGraphicsItem>
#include <QPropertyAnimation>
#include <QObject>

class DragRegisterIndicator : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    float animationPhase; // TODO: Do I need this?
    QPropertyAnimation animation;
    QPointF endPos;
    bool hits;
    bool suitable;

public:
    DragRegisterIndicator();
    float getanimationPhase() const;
    void setanimationPhase(float newAnimationPhase);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;
    void setEnd(QPointF pos, bool hits, bool s);
    void doSuccessAnimation();
    void abortAnimation();

private:
    void paintArrowHead(QPainter *painter, float size);
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)

signals:
    void animationPhaseChanged();

private slots:
    void endOfAnimation();
};

#endif // DRAGREGISTERINDICATOR_H
