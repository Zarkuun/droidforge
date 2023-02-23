#include "windowlist.h"
#include "globals.h"
#include "mainwindow.h"
#include "tuning.h"

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
    for (auto window: windows)
        menu->addAction(window->btfAction());
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
        xMax = qMax(xMax, window->pos().x());
        yMax = qMax(yMax, window->pos().y());
    }
    return QPoint(xMax + WINDOW_X_DISPLACEMENT,
                  yMax + WINDOW_Y_DISPLACEMENT);
}
MainWindow *WindowList::windowWithFile(const QString &filePath)
{
    QFileInfo info(filePath);
    QString absolute = info.absoluteFilePath();
    for (auto window: windows) {
        QString p = window->getFilePath();
        if (p != "") {
            QFileInfo winfo(p);
            if (winfo.absoluteFilePath() == absolute)
                return window;
        }
    }
    return 0;
}
void WindowList::saveAll()
{
    for (auto window: windows)
        window->saveMeLikeAll();
}
