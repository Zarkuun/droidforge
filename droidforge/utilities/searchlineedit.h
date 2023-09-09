#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    SearchLineEdit(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void keyPressed(QKeyEvent *event);
};

#endif // SEARCHLINEEDIT_H
