#ifndef FRAMECURSOR_H
#define FRAMECURSOR_H

#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QObject>

typedef enum {
    CURSOR_NORMAL,
    CURSOR_PROBLEM,
    CURSOR_PATCHING,
    CURSOR_DISABLED,
} cursor_mode_t;

class MainWindow;

class FrameCursor : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

    MainWindow *mainWindow;
    float animationPhase;
    QPropertyAnimation animation;
    cursor_mode_t mode;
    QRectF lastRect;
    QColor color;
    bool dotted;

public:
    FrameCursor(MainWindow *mainWindow);
    ~FrameCursor();
    void setMode(cursor_mode_t m);
    void setDotted(bool d) { dotted = d; };
    float getanimationPhase() const;
    void setanimationPhase(float newanimationPhase);
    void startAnimation();

signals:
    void animationPhaseChanged();

private:
    Q_PROPERTY(float animationPhase READ getanimationPhase WRITE setanimationPhase NOTIFY animationPhaseChanged)
    bool textMode() const;
};

#endif // FRAMECURSOR_H
