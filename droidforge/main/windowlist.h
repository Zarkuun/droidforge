#ifndef WINDOWLIST_H
#define WINDOWLIST_H

#include <QMenu>
#include <QObject>
#include <QList>

class MainWindow;

class WindowList : public QObject
{
    Q_OBJECT

    QList<MainWindow *> windows;

public:
    WindowList();
    void add(MainWindow *window);
    void remove(MainWindow *window);
    void addMenuEntries(QMenu *menu);
    void update();
    MainWindow *nextWindow(const MainWindow *window);
    MainWindow *previousWindow(const MainWindow *window);

signals:
    void changed();
};

extern WindowList *the_windowlist;

#endif // WINDOWLIST_H
