#include "windowlist.h"
#include "globals.h"
#include "mainwindow.h"

WindowList *the_windowlist = 0;

WindowList::WindowList()
{
    the_windowlist = this;
}
void WindowList::add(MainWindow *window)
{
    windows.append(window);
    emit changed();
}
void WindowList::remove(MainWindow *window)
{
    windows.removeAll(window);
    emit changed();
}
void WindowList::addMenuEntries(QMenu *menu)
{
    for (auto window: windows) {
        QString title = window->patchName();
        QAction *action = new QAction(title, this);
        connect(action, &QAction::triggered, this, [window]() { window->bringToFront();});
        menu->addAction(action);
    }
}
void WindowList::update()
{
    emit changed();
}
MainWindow *WindowList::nextWindow(const MainWindow *window)
{
    int index = windows.indexOf(window);
    if (index < 0) // not found: should not happen
        return windows[0];
    else if (index >= windows.count() - 1) // at the end
        return windows[0];
    else
        return windows[index+1];
}

MainWindow *WindowList::previousWindow(const MainWindow *window)
{
    int index = windows.indexOf(window);
    if (index < 0) // not found: should not happen
        return windows[windows.count() - 1];
    else if (index == 0) // at the beginning
        return windows[windows.count() - 1];
    else
        return windows[index-1];
}

QPoint WindowList::newPosition() const
{
    int xMax = 0;
    int yMax = 0;
    for (auto window: windows) {
        shout << "Fenster bei" << window->pos();
        xMax = qMax(xMax, window->pos().x());
        yMax = qMax(yMax, window->pos().y());
    }
#define WINDOW_X_DISPLACEMENT 30
#define WINDOW_Y_DISPLACEMENT 10
    return QPoint(xMax + WINDOW_X_DISPLACEMENT,
                  yMax + WINDOW_Y_DISPLACEMENT);

}
