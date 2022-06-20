#ifndef JACKSELECTOR_H
#define JACKSELECTOR_H

#include "droidfirmware.h"
#include "jackview.h"
#include "jackassignment.h"

#include <QGraphicsView>
#include <QString>

class JackSelector : public QGraphicsView
{
    Q_OBJECT
    QString circuit;
    QList<JackView *> jackViews[2];
    int currentRow;
    int currentColumn;
    int currentSubjack;
    const QStringList *usedJacks;
    jacktype_t jackType;

public:
    JackSelector(QWidget *parent);
    void setCircuit(const QString &ircuit, const QString &current, const QStringList &usedJacks, jacktype_t, QString search);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    QString getSelectedJack() const;

private:
    void initScene();
    void loadJacks(QString circuit, QString search="");
    unsigned createJacks(const QStringList &jacks, int column);
    void placeJacks(int totalHeight, float space, int column);
    void moveCursorUpDown(int whence);
    void moveCursorLeftRight(int whence);
    JackView *currentJack();
    const JackView *currentJack() const;
    void selectCurrentJack(bool sel);
    bool handleMousePress(const QPointF &pos);
    void setCursor(QString current = "");

signals:
    void cursorMoved(QString jack, jacktype_t jacktype, bool onActive);
    void accepted();

public slots:
    void searchChanged(QString text);
};

#endif // JACKSELECTOR_H
