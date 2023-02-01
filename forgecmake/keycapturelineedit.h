#ifndef KEYCAPTURELINEEDIT_H
#define KEYCAPTURELINEEDIT_H

#include <QLineEdit>

class KeyCaptureLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    KeyCaptureLineEdit(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void keyPressed(int key);
};

#endif // KEYCAPTURELINEEDIT_H
