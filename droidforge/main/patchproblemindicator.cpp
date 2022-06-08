#include "patchproblemindicator.h"
#include "editoractions.h"
#include "patchoperator.h"
#include "tuning.h"
#include "updatehub.h"

#include <QPainter>
#include <QMouseEvent>

PatchProblemIndicator::PatchProblemIndicator(PatchEditEngine *patch, QWidget *parent)
    : QWidget{parent}
    , PatchView(patch)
    , warningImage(":images/icons/warning.png") // TODO: Zentral ablegen?
    , okImage(":images/icons/white/sentiment_very_satisfied.png") // TODO: Hilfsfunktion
    , numProblems(0)
    , currentProblem(0)
{
    setMinimumWidth(PPI_WIDTH);
    setMaximumWidth(PPI_WIDTH);

    CONNECT_ACTION(ACTION_TOOLBAR_PROBLEMS, &PatchProblemIndicator::jumpToNextProblem);
    CONNECT_ACTION(ACTION_JUMP_TO_NEXT_PROBLEM, &PatchProblemIndicator::jumpToNextProblem);
    SET_ACTION_TRIGGER(ACTION_JUMP_TO_NEXT_PROBLEM, &PatchProblemIndicator::clicked);

    // Events that we create
    connect(this, &PatchProblemIndicator::patchModified, the_hub, &UpdateHub::modifyPatch);
    connect(this, &PatchProblemIndicator::sectionSwitched, the_hub, &UpdateHub::switchSection);
    connect(this, &PatchProblemIndicator::cursorMoved, the_hub, &UpdateHub::moveCursor);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::patchModified, this, &PatchProblemIndicator::updateStatus);
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

void PatchProblemIndicator::updateStatus()
{
    numProblems = patch->numProblems();
    if (numProblems) {
        setToolTip(tr("Your patch has %1 problems. You need to fix then "
                      "before you can load it to your master. Click here "
                      "to jump to the first problem.").arg(numProblems));
        setCursor(Qt::PointingHandCursor);
    }
    else {
        setToolTip(tr("Your patch does not have any problems. It is ready "
                      "to be loaded to your master."));
        unsetCursor();
    }
    update();
}

void PatchProblemIndicator::jumpToNextProblem()
{
    if (patch->numProblems() == 0)
        return;

    const PatchProblem *problem = patch->problem(currentProblem++);
    if (currentProblem >= patch->numProblems())
        currentProblem = 0;
    the_operator->jumpTo(problem->getSection(), problem->getCursorPosition());
}
