#include "patchproblemindicator.h"
#include "tuning.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

PatchProblemIndicator::PatchProblemIndicator(QWidget *parent)
    : QWidget{parent}
    , warningImage(":images/icons/warning.png") // TODO: Zentral ablegen?
    , okImage(":images/icons/white/sentiment_very_satisfied.png") // TODO: Hilfsfunktion
    , numProblems(0)
{
    setMinimumWidth(PPI_WIDTH);
    setMaximumWidth(PPI_WIDTH);
}

void PatchProblemIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), PPI_BACKGROUND_COLOR);

    unsigned imgHeight = height() - 2 * PPI_IMAGE_MARGIN;

    QRectF warnRect(PPI_SIDE_PADDING,
                    PPI_IMAGE_MARGIN,
                    imgHeight,
                    imgHeight);

    painter.drawImage(warnRect, numProblems ? warningImage : okImage);

    float textLeft = warnRect.right() + STANDARD_SPACING;
    QRectF textRect(textLeft, 0, width() - textLeft, height());
    QString text = tr("%1 problems").arg(numProblems);
    painter.drawText(textRect, Qt::AlignVCenter, text);
}

void PatchProblemIndicator::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
}

void PatchProblemIndicator::problemsChanged(unsigned count)
{
    numProblems = count;
    if (count) {
        setToolTip(tr("Your patch has %1 problems. You need to fix then "
                      "before you can load it to your master. Click here "
                      "to jump to the first problem.").arg(count));
        setCursor(Qt::PointingHandCursor);
    }
    else {
        setToolTip(tr("Your patch does not have any problems. It is ready "
                      "to be loaded to your master."));
        unsetCursor();
    }
    update();
}
