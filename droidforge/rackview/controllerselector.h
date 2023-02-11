#ifndef CONTROLLERSELECTOR_H
#define CONTROLLERSELECTOR_H

#include "modulebuilder.h"

#include <QGraphicsView>
#include <QString>
#include <QGraphicsRectItem>

class MainWindow;
class ModuleBuilder;

class ControllerSelector : public QGraphicsView
{
    Q_OBJECT
    MainWindow *mainWindow;
    ModuleBuilder moduleBuilder;
    QString selectedController;
    QGraphicsRectItem *cursor;

public:
    explicit ControllerSelector(MainWindow *mainWindow, QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    const QString &getSelectedController() const { return selectedController; };
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void moveCursor(int whence);
    void moveCursorHome();
    void moveCursorEnd();

private:
    void placeCursor();

signals:
    void controllerSelected(QString name);
};

#endif // CONTROLLERSELECTOR_H
