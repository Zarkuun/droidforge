#ifndef PATCHPROBLEMINDICATOR_H
#define PATCHPROBLEMINDICATOR_H

#include <QWidget>

class PatchProblemIndicator : public QWidget
{
    Q_OBJECT

    QImage warningImage;
    QImage okImage;
    unsigned numProblems;

public:
    PatchProblemIndicator(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

public slots:
    void problemsChanged(unsigned count);

signals:
    void clicked();
};


#endif // PATCHPROBLEMINDICATOR_H
