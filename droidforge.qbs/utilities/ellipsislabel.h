#ifndef ELLIPSISLABEL_H
#define ELLIPSISLABEL_H

#include <QLabel>

class EllipsisLabel : public QLabel
{
    Q_OBJECT
    QString text;

public:
    explicit EllipsisLabel(QWidget *parent = nullptr);
    explicit EllipsisLabel(QString text, QWidget *parent = nullptr);
    void setText(QString);

protected:
    void resizeEvent(QResizeEvent *);

private:
    void updateText();
};

#endif // ELLIPSISLABEL_H
