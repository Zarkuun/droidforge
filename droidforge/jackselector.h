#ifndef JACKSELECTOR_H
#define JACKSELECTOR_H

#include "droidfirmware.h"
#include "jackview.h"

#include <QGraphicsView>
#include <QString>

class JackSelector : public QGraphicsView
{
    Q_OBJECT
    QString circuit;
    QList<JackView *> jackViews[2];
    int currentRow;
    int currentColumn;

public:
    JackSelector(QWidget *parent);
    void setCircuit(const QString &ircuit);
    void keyPressEvent(QKeyEvent *event);
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
};

#endif // JACKSELECTOR_H
