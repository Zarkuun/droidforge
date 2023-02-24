#ifndef CABLESELECTORLINEEDIT_H
#define CABLESELECTORLINEEDIT_H

#include <QLineEdit>
#include <QObject>

class CableSelectorLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    CableSelectorLineEdit(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void keyPressed(int key);
};

#endif // CABLESELECTORLINEEDIT_H
