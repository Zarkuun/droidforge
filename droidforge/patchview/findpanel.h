#ifndef FINDPANEL_H
#define FINDPANEL_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>

#include "searchlineedit.h"

class FindPanel : public QWidget
{
    Q_OBJECT
    QLabel *label;
    SearchLineEdit *searchField;
    QToolButton *buttonPrev;
    QToolButton *buttonNext;
    QToolButton *buttonFinished;

public:
    explicit FindPanel(QWidget *parent = nullptr);

protected:
    virtual void showEvent(QShowEvent *) override;

private:
    void doSearch(int direction);

public slots:
    void updateSearchStats(unsigned pos, unsigned count);

private slots:
    void searchForward();
    void searchBackward();
    void catchKeyPress(QKeyEvent *event);
    void finished();

signals:
    void search(QString text, int direction);
    void keyCaptured(QKeyEvent *event);
};

#endif // FINDPANEL_H
