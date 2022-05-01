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
    unsigned circuitViewWidth;

public:
    CircuitCollection(QString category, QWidget *parent);
    CircuitCollection(QWidget *parent);
    ~CircuitCollection();
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    QString selectedCircuitName();
    void updateSearch(QString text);
    void resizeEvent(QResizeEvent* event);

private:
    void initScene();
    void initBoundingRect(int numCircuits);
    bool handleMousePress(const QPointF &pos);
    void loadCircuitCategory(QString category, QString search="");
    void moveCursorUpDown(int whence);
    CircuitInfoView *currentCircuit();
    void chooseCurrentCircuit();

signals:
    void selectCircuit();
};

#endif // CIRCUITCOLLECTION_H
