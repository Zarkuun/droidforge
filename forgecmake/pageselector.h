#ifndef PAGESELECTOR_H
#define PAGESELECTOR_H

#include "os.h"

#include "keycapturelineedit.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPdfDocument;
class QPdfPageNavigator;
class QToolButton;
QT_END_NAMESPACE

class PageSelector : public QWidget
{
    Q_OBJECT

    QPdfPageNavigator *pageNavigator;
    KeyCaptureLineEdit *lineEditPage;
    QLabel *labelPageCount;
    QToolButton *buttonPrev;
    QToolButton *buttonNext;
    int pageCount;

public:
    explicit PageSelector(int pageCount, QWidget *parent = nullptr);
    void setPageNavigator(QPdfPageNavigator *pageNavigator);
    void goToPage(int page, bool withHistory);

private slots:
    void onCurrentPageChanged(int page);
    void pageNumberEdited();
    void handleKeyPress(int key);
};

#endif // PAGESELECTOR_H
