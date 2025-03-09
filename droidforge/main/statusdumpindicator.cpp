#include "statusdumpindicator.h"
#include "mainwindow.h"
#include "patchoperator.h"

#include <QPainter>


StatusDumpIndicator::StatusDumpIndicator(MainWindow *mainWindow, PatchEditEngine *patch, QWidget *parent)
    : QWidget{parent}
    , PatchView(patch)
    , mainWindow(mainWindow)
    , dumpIndex(-1)
    , dumpCount(0)
{
    label = new QLabel();
    label->setText("Status dump 1");

    buttonPrev = new QToolButton(this);
    buttonPrev->setText("<");
    buttonPrev->setToolTip(tr("Switch to the previous status dump"));

    buttonNext = new QToolButton(this);
    buttonNext->setText(">");
    buttonNext->setToolTip(tr("Switch to the next status dump"));

    buttonInfo = new QToolButton(this);
    buttonInfo->setText(tr("info"));
    buttonInfo->setToolTip(tr("Show the file contents of the current status dump"));

    buttonClose = new QToolButton(this);
    buttonClose->setText("X");
    buttonClose->setToolTip(tr("Close the status dump, back to normal view"));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(buttonPrev);
    mainLayout->addWidget(buttonNext);
    mainLayout->addWidget(buttonInfo);
    mainLayout->addWidget(buttonClose);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Events that we are interested in
    // connect(mainWindow->theHub(), &UpdateHub::patchModified, this, &StatusDumpIndicator::updateStatus);

    connect(buttonPrev, &QToolButton::clicked, this, &StatusDumpIndicator::switchPrev);
    connect(buttonNext, &QToolButton::clicked, this, &StatusDumpIndicator::switchNext);
    connect(buttonInfo, &QToolButton::clicked, mainWindow->theOperator(), &PatchOperator::showStatusDumpInfo);
    connect(buttonClose, &QToolButton::clicked, this, &StatusDumpIndicator::hideDumps);
    hide();
}

void StatusDumpIndicator::updateStatus(unsigned int numStatusDumps, int index)
{
    dumpIndex = index;
    dumpCount = numStatusDumps;

    if (dumpCount == 0)
        hide();
    else
    {
        label->setText(tr("Status dump #%1").arg(index + 1));
        if (dumpCount > 1) {
            buttonNext->show();
            buttonNext->setEnabled(index+1 < (int)numStatusDumps);
            buttonPrev->show();
            buttonPrev->setEnabled(index > 0);
        }
        else {
            buttonNext->hide();
            buttonPrev->hide();
        }
        show();
    }
}
void StatusDumpIndicator::switchPrev()
{
    dumpIndex --;
    if (dumpIndex < 0)
        dumpIndex = dumpCount - 1;
    mainWindow->theOperator()->showStatusDumpNr(dumpIndex);
}
void StatusDumpIndicator::switchNext()
{
    dumpIndex ++;
    if (dumpIndex >= (int)dumpCount)
        dumpIndex = 0;
    mainWindow->theOperator()->showStatusDumpNr(dumpIndex);
}
void StatusDumpIndicator::showDumpInfo()
{
}
void StatusDumpIndicator::hideDumps()
{
    mainWindow->theOperator()->showStatusDumpNr(-1);
}
