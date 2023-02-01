#ifndef CLIPBOARDINDICATOR_H
#define CLIPBOARDINDICATOR_H

#include "clipboard.h"

#include <QWidget>

class ClipboardIndicator : public QWidget
{
    Q_OBJECT
    QImage pasteImage;

public:
    explicit ClipboardIndicator(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

public slots:
    void changeClipboard();

signals:
    void clicked();
};

#endif // CLIPBOARDINDICATOR_H
