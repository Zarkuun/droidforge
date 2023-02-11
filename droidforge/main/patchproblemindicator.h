#ifndef PATCHPROBLEMINDICATOR_H
#define PATCHPROBLEMINDICATOR_H

#include <QWidget>
#include "patchview.h"

class MainWindow;
class PatchOperator;

class PatchProblemIndicator : public QWidget, PatchView
{
    Q_OBJECT

    MainWindow *mainWindow;
    unsigned numProblems;
    unsigned currentProblem;

public:
    PatchProblemIndicator(MainWindow *mainWindow, PatchEditEngine *patch, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    PatchOperator *theOperator();

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
