#ifndef CLIPBOARDINDICATOR_H
#define CLIPBOARDINDICATOR_H

#include "clipboard.h"

#include <QWidget>

class MainWindow;

class ClipboardIndicator : public QWidget
{
    Q_OBJECT
    MainWindow *mainWindow;
    QImage pasteImage;

public:
    explicit ClipboardIndicator(MainWindow *mainWindow);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

public slots:
    void changeClipboard();

signals:
    void clicked();
};

#endif // CLIPBOARDINDICATOR_H
