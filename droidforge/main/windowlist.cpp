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
void WindowList::debug()
{
   shout << "Windows:";
   for (auto window: windows) {
       shout << window->patchName();
   }
}
void WindowList::addMenuEntries(QMenu *menu)
{
    shoutfunc;
    for (auto window: windows) {
        QString title = window->patchName();
        QAction *action = new QAction(title, this);
        connect(action, &QAction::triggered, this, [window]() { window->raise(); window->activateWindow();});
        menu->addAction(action);
    }
}

void WindowList::update()
{
    emit changed();
}
