#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Dialog : public QDialog
{
    QString id;
    bool closeOnReturn;

public:
    Dialog(QString id, QWidget *parent = nullptr);
    void setCloseOnReturn(bool cr) { closeOnReturn = cr; };

protected:
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    void keyPressEvent(QKeyEvent *event);
    void setDefaultSize(QSize s);
};

#endif // DIALOG_H
