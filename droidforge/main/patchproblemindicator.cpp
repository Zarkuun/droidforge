#include "patchproblemindicator.h"
#include "colorscheme.h"
#include "editoractions.h"
#include "patchoperator.h"
#include "tuning.h"
#include "updatehub.h"
#include "iconbase.h"
#include "mainwindow.h"

#include <QPainter>
#include <QMouseEvent>

PatchProblemIndicator::PatchProblemIndicator(MainWindow *mainWindow, PatchEditEngine *patch, QWidget *parent)
    : QWidget{parent}
    , PatchView(patch)
    , mainWindow(mainWindow)
    , numProblems(0)
    , currentProblem(0)
{
    setMinimumWidth(PPI_WIDTH);
    setMaximumWidth(PPI_WIDTH);

    CONNECT_ACTION(ACTION_TOOLBAR_PROBLEMS, &PatchProblemIndicator::jumpToNextProblem);
    CONNECT_ACTION(ACTION_JUMP_TO_NEXT_PROBLEM, &PatchProblemIndicator::jumpToNextProblem);
    SET_ACTION_TRIGGER(ACTION_JUMP_TO_NEXT_PROBLEM, &PatchProblemIndicator::clicked);

    // Events that we create
    connect(this, &PatchProblemIndicator::patchModified, mainWindow->theHub(), &UpdateHub::modifyPatch);
    connect(this, &PatchProblemIndicator::sectionSwitched, mainWindow->theHub(), &UpdateHub::switchSection);
    connect(this, &PatchProblemIndicator::cursorMoved, mainWindow->theHub(), &UpdateHub::moveCursor);

    // Events that we are interested in
    connect(mainWindow->theHub(), &UpdateHub::patchModified, this, &PatchProblemIndicator::updateStatus);
}

void PatchProblemIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), COLOR(COLOR_STATUSBAR_BACKGROUND));

    unsigned imgHeight = height() - 2 * PPI_IMAGE_MARGIN;

    QRectF warnRect(PPI_SIDE_PADDING,
                    PPI_IMAGE_MARGIN,
                    imgHeight,
                    imgHeight);

    if (numProblems)
        painter.drawImage(warnRect, IMAGE("problemsmall"));

    float textLeft = warnRect.right() + STANDARD_SPACING;
    QRectF textRect(textLeft, 0, width() - textLeft, height());
    QString text = tr("%1 problems").arg(numProblems);
    painter.setPen(COLOR(COLOR_STATUSBAR_TEXT));
    painter.drawText(textRect, Qt::AlignVCenter, text);
}
void PatchProblemIndicator::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();
}
PatchOperator *PatchProblemIndicator::theOperator()
{
    return mainWindow->theOperator();
}
void PatchProblemIndicator::updateStatus()
{
    numProblems = patch->numProblems();
    if (numProblems) {
        setToolTip(tr("Your patch has %1 problems. You need to fix them "
                      "before you can load it to your master. Click here "
                      "to jump to the next problem.").arg(numProblems));
        setCursor(Qt::PointingHandCursor);
    }
    else {
        setToolTip(tr("Your patch does not have any problems. It is ready "
                      "to be loaded to your master."));
        unsetCursor();
    }
    ACTION(ACTION_TOOLBAR_PROBLEMS)->setToolTip(toolTip());
    update();
}
void PatchProblemIndicator::jumpToNextProblem()
{
    if (patch->numProblems() == 0)
        return;

    if (currentProblem >= patch->numProblems())
        currentProblem = 0;
    const PatchProblem *problem = patch->problem(currentProblem++);
    if (currentProblem >= patch->numProblems())
        currentProblem = 0;
    theOperator()->jumpTo(problem->getSection(), problem->getCursorPosition());
}
