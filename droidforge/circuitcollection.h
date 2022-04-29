#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include "circuitinfoview.h"

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{
    Q_OBJECT

    QGraphicsRectItem *backgroundRect;
    QList<CircuitInfoView *>circuits;
    int numCircuits;
    int selectedCircuit;

public:
    CircuitCollection(QString category, QWidget *parent);
    ~CircuitCollection();
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QString selectedCircuitName();

private:
    bool handleMousePress(const QPointF &pos);
    unsigned loadCircuitCategory(QString category);
    void moveCursorUpDown(int whence);
    CircuitInfoView *currentCircuit();
    void chooseCurrentCircuit();

signals:
    void selectCircuit();
};

#endif // CIRCUITCOLLECTION_H
