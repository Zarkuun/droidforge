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
    void debug();
    void addMenuEntries(QMenu *menu);
    void update();

signals:
    void changed();
};

extern WindowList *the_windowlist;

#endif // WINDOWLIST_H
