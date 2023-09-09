#include "searchlineedit.h"
#include "globals.h"

#include <QKeyEvent>


SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
}

void SearchLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (hasSelectedText()) {
        int key = event->key();
        if (key == Qt::Key_Up
                || key == Qt::Key_Down
                || key == Qt::Key_Left
                || key == Qt::Key_Right
                || key == Qt::Key_Home
                || key == Qt::Key_End
                )
        {
            emit keyPressed(event);
            return;
        }
    }
    QLineEdit::keyPressEvent(event);
}
