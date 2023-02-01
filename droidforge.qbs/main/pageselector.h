#ifndef PAGESELECTOR_H
#define PAGESELECTOR_H

#include "os.h"

#include "keycapturelineedit.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPdfDocument;
class QPdfPageNavigation;
class QToolButton;
QT_END_NAMESPACE

class PageSelector : public QWidget
{
    Q_OBJECT

    QPdfPageNavigation *pageNavigation;
    KeyCaptureLineEdit *lineEditPage;
    QLabel *labelPageCount;
    QToolButton *buttonPrev;
    QToolButton *buttonNext;

public:
    explicit PageSelector(QWidget *parent = nullptr);
    void setPageNavigation(QPdfPageNavigation *pageNavigation);

private slots:
    void onCurrentPageChanged(int page);
    void pageNumberEdited();
    void handleKeyPress(int key);
};

#endif // PAGESELECTOR_H
