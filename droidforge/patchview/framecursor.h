#ifndef FRAMECURSOR_H
#define FRAMECURSOR_H

#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QObject>

typedef enum {
    CURSOR_NORMAL,
    CURSOR_PROBLEM,
    CURSOR_PATCHING,
} cursor_mode_t;


class FrameCursor : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

    float animationPhase; // TODO: Do I need this?
    QPropertyAnimation animation;
    cursor_mode_t mode;

public:
    FrameCursor();
    ~FrameCursor();
    void setMode(cursor_mode_t m);
    float getanimationPhase() const;
    void setanimationPhase(float newanimationPhase);
    void startAnimation();

signals:
    void animationPhaseChanged(); // TODO: needed?

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
};

#endif // FRAMECURSOR_H