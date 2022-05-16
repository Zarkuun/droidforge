#ifndef CONTROLLERSELECTOR_H
#define CONTROLLERSELECTOR_H

#include <QGraphicsView>
#include <QString>

class ControllerSelector : public QGraphicsView
{
    Q_OBJECT
    QString selectedController;

public:
    explicit ControllerSelector(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    const QString &getSelectedController() const { return selectedController; };
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void controllerSelected(QString name);
};

#endif // CONTROLLERSELECTOR_H
