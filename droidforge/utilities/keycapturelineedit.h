#ifndef KEYCAPTURELINEEDIT_H
#define KEYCAPTURELINEEDIT_H

#include <QLineEdit>

typedef enum {
    SPACE_ALLOWED,
    SPACE_IGNORED,
    SPACE_BYPASSED,
    SPACE_TO_UNDERSCORE,
} spacehanlding_t;

class KeyCaptureLineEdit : public QLineEdit
{
    Q_OBJECT
    spacehanlding_t spaceHandling;

public:
    KeyCaptureLineEdit(QWidget *parent = nullptr);
    void setSpaceHandling(spacehanlding_t s) { spaceHandling = s; };

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void keyPressed(int key);
};

#endif // KEYCAPTURELINEEDIT_H
