#ifndef CABLESTATUSINDICATOR_H
#define CABLESTATUSINDICATOR_H

#include <QWidget>
#include <QPainter>

class CableStatusIndicator : public QWidget
{
    Q_OBJECT
    QString cableName;
    int numAsInput;
    int numAsOutput;

public:
    explicit CableStatusIndicator(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void set(QString name, int numAsInput, int numAsOutput);
    void clear();

signals:

};

#endif // CABLESTATUSINDICATOR_H
