#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include "circuitinfoview.h"

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{
    Q_OBJECT

    QList<CircuitInfoView *>circuits;
    int numCircuits;
    int selectedCircuit;
    unsigned circuitViewWidth;

public:
    CircuitCollection(QString category, QWidget *parent);
    CircuitCollection(QWidget *parent);
    ~CircuitCollection();
    unsigned getNumCircuits() const { return numCircuits; };
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString selectedCircuitName();
    void updateSearch(QString text);
    void resizeEvent(QResizeEvent* event);
    bool preselectCircuit(QString name);
    void moveCursorUpDown(int whence);

private:
    void initScene();
    void updateSceneRect(int numCircuits);
    bool handleMousePress(const QPoint &pos);
    void loadCircuitCategory(QString category, QString search="");
    CircuitInfoView *currentCircuit();
    void chooseCurrentCircuit();

signals:
    void selectCircuit();
};

#endif // CIRCUITCOLLECTION_H
