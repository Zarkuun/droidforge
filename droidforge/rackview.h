#ifndef RACKVIEW_H
#define RACKVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>


class RackView : public QGraphicsView
{
    Q_OBJECT

private:
    QGraphicsScene *scene;

public:
    explicit RackView(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);

signals:

};

#endif // RACKVIEW_H
