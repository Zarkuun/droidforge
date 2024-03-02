#include "keycapturelineedit.h"

#include <QKeyEvent>

KeyCaptureLineEdit::KeyCaptureLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , spaceHandling(SPACE_BYPASSED)
{
}

void KeyCaptureLineEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Space) {
        switch (spaceHandling) {
            case SPACE_ALLOWED:
                QLineEdit::keyPressEvent(event);
                return;

            case SPACE_IGNORED:
                return;

            case SPACE_BYPASSED:
                emit keyPressed(event->key());
                return;

            case SPACE_TO_UNDERSCORE: {
                QLineEdit::insert("_");
                return;
            }
        }
    }

    else if (key == Qt::Key_Up
        || key == Qt::Key_Down
        || key == Qt::Key_Left
        || key == Qt::Key_Right
        || key == Qt::Key_Home
        || key == Qt::Key_End
    )
    {
       emit keyPressed(event->key());
    }
    else
       QLineEdit::keyPressEvent(event);
}
