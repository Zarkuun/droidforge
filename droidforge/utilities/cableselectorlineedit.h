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

private slots:
    void changeText(const QString &t);

signals:
    void keyPressed(int key);
};

#endif // CABLESELECTORLINEEDIT_H
