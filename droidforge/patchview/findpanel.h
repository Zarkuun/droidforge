#ifndef FINDPANEL_H
#define FINDPANEL_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QToolButton>

class FindPanel : public QWidget
{
    Q_OBJECT
    QLineEdit *searchField;
    QToolButton *buttonPrev;
    QToolButton *buttonNext;
    QToolButton *buttonFinished;

public:
    explicit FindPanel(QWidget *parent = nullptr);

protected:
    virtual void showEvent(QShowEvent *) override;
    // bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // void handleKeyPress(int key);
    void doSearch(int direction);

private slots:
    void searchForward();
    void searchBackward();

signals:
    void search(QString text, int direction);

};

#endif // FINDPANEL_H
