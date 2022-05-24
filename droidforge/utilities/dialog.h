#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Dialog : public QDialog
{
    QString id;

public:
    Dialog(QString id, QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
};

#endif // DIALOG_H
