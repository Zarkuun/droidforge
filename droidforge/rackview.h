#ifndef RACKVIEW_H
#define RACKVIEW_H

#include "rack.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>


class RackView : public QGraphicsView
{
    Q_OBJECT
    Rack *rack;

public:
    explicit RackView(Rack *arack);
    void resizeEvent(QResizeEvent *event);

private:
    void buildRack();

signals:

};

#endif // RACKVIEW_H
