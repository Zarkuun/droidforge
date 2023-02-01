#include "ellipsislabel.h"
#include "globals.h"

EllipsisLabel::EllipsisLabel(QWidget *parent)
    : EllipsisLabel("", parent)
{
}

EllipsisLabel::EllipsisLabel(QString text, QWidget *parent)
    : QLabel(parent)
{
    setText(text);
}

void EllipsisLabel::setText(QString newtext)
{
    text = newtext;
    updateText();
}

void EllipsisLabel::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    updateText();
}

void EllipsisLabel::updateText()
{
    QFontMetrics metrics(font());
    QString elided = metrics.elidedText(text, Qt::ElideRight, width());
    QLabel::setText(elided);
}
