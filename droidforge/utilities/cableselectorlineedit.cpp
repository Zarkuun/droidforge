#include "cableselectorlineedit.h"
#include "globals.h"

#include <QKeyEvent>
#include <QRegularExpression>

CableSelectorLineEdit::CableSelectorLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    connect(this, &QLineEdit::textChanged, this, &CableSelectorLineEdit::changeText);

}
void CableSelectorLineEdit::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Down)
       emit keyPressed(event->key());
    else
        QLineEdit::keyPressEvent(event);
}
void CableSelectorLineEdit::changeText(const QString &)
{
    static QRegularExpression re("[^A-Z0-9_]*");
    int curPos = cursorPosition();
    QString text = this->text().toUpper().replace(' ', '_');
    text = text.replace(re, "");
    setText(text);
    setCursorPosition(curPos);
}
