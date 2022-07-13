#ifndef PATCHPROBLEMINDICATOR_H
#define PATCHPROBLEMINDICATOR_H

#include <QWidget>
#include "patchview.h"

class PatchProblemIndicator : public QWidget, PatchView
{
    Q_OBJECT

    unsigned numProblems;
    unsigned currentProblem;

public:
    PatchProblemIndicator(PatchEditEngine *patch, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

public slots:
    void updateStatus();
    void jumpToNextProblem();

signals:
    void patchModified();
    void sectionSwitched();
    void cursorMoved();
    void clicked();
};


#endif // PATCHPROBLEMINDICATOR_H
