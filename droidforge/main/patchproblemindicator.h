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

public:
    PatchProblemIndicator(PatchEditEngine *patch, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

public slots:
    void problemsChanged(unsigned count);

signals:
    void clicked();
};


#endif // PATCHPROBLEMINDICATOR_H
