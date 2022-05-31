#include "clipboardindicator.h"
#include "tuning.h"
#include "updatehub.h"

#include <QMouseEvent>

ClipboardIndicator::ClipboardIndicator(QWidget *parent)
    : QWidget{parent}
    , pasteImage(":images/icons/white/description.png")
{
    setMinimumWidth(CI_WIDTH);
    setMaximumWidth(CI_WIDTH);
    setVisible(false); // wait until first clipboard event

    // Events that we are interested in
    connect(the_hub, &UpdateHub::clipboardChanged, this, &ClipboardIndicator::changeClipboard);
}

void ClipboardIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), CI_BACKGROUND_COLOR);
    if (the_clipboard->isEmpty()) {
        setMaximumWidth(0);
        return;
    }

    unsigned imgHeight = height() - 2 * CI_IMAGE_MARGIN;
    QRectF imgRect(CI_SIDE_PADDING,
                   CI_IMAGE_MARGIN,
                   imgHeight,
                   imgHeight);
    painter.drawImage(imgRect, pasteImage);

    float textLeft = imgRect.right() + STANDARD_SPACING;
    QRectF textRect(textLeft, 0, width() - textLeft, height());

    QString info;
    if (the_clipboard->isEmpty())
        info = "";
    else if (the_clipboard->numCircuits())
        info = tr("%1 circuits").arg(the_clipboard->numCircuits());
    else if (the_clipboard->numJacks())
        info = tr("%1 jacks").arg(the_clipboard->numJacks());
    else if (the_clipboard->numAtoms())
        info = tr("%1 parameters").arg(the_clipboard->numAtoms());
    else if (the_clipboard->isComment())
        info = tr("comment");

    painter.drawText(textRect, Qt::AlignVCenter, info);
}

void ClipboardIndicator::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
}

void ClipboardIndicator::changeClipboard()
{
    if (the_clipboard->isEmpty())
        setVisible(false);
    else
        setVisible(true);
    update();
}
