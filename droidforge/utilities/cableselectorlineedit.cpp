#include "cableselectorlineedit.h"
#include "globals.h"

#include <QKeyEvent>

CableSelectorLineEdit::CableSelectorLineEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

void CableSelectorLineEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Down)
       emit keyPressed(event->key());
    else
       QLineEdit::keyPressEvent(event);
}
