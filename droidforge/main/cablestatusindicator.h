#ifndef CABLESTATUSINDICATOR_H
#define CABLESTATUSINDICATOR_H

#include <QWidget>
#include <QPen>
#include <QPainter>

class CableStatusIndicator : public QWidget
{
    Q_OBJECT
    QPen cablePen;
    QPen cableHilitePen;
    QString cableName;
    QImage warningImage;

    int numAsInput;
    int numAsOutput;

public:
    explicit CableStatusIndicator(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);

    // TODO: Das set/clear besser mit Slot/Signal machen.
    // PatchView oder SectionView->signal atomchanged...
    // Das hier verbinden
    void set(QString name, int numAsInput, int numAsOutput);
    void clear();

private:

signals:

};

#endif // CABLESTATUSINDICATOR_H