#ifndef CIRCUITCOLLECTION_H
#define CIRCUITCOLLECTION_H

#include <QGraphicsView>
#include <QObject>

class CircuitCollection : public QGraphicsView
{
    QString title;

public:
    CircuitCollection(const QString &title, QWidget *parent);
    ~CircuitCollection();
    const QString &getTitle() const { return title; };
};

#endif // CIRCUITCOLLECTION_H
