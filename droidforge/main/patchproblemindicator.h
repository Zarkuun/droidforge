#ifndef PATCHPROBLEMINDICATOR_H
#define PATCHPROBLEMINDICATOR_H

#include <QWidget>
#include "patchoperator.h"

class PatchProblemIndicator : public QWidget, PatchOperator
{
    Q_OBJECT

    QImage warningImage;
    QImage okImage;
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
    void sectionSwitched();
    void cursorMoved();
    void clicked();
};


#endif // PATCHPROBLEMINDICATOR_H
