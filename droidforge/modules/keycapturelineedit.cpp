#include "keycapturelineedit.h"
#include "globals.h"

#include <QKeyEvent>

KeyCaptureLineEdit::KeyCaptureLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}

void KeyCaptureLineEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Up
        || key == Qt::Key_Down
        || key == Qt::Key_Left
        || key == Qt::Key_Right
        || key == Qt::Key_Home
        || key == Qt::Key_End
        || key == Qt::Key_Space)
    {
       emit keyPressed(event->key());
    }
    else
       QLineEdit::keyPressEvent(event);
}
