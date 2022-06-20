#include "dialog.h"

#include <QSettings>
#include <QKeyEvent>

Dialog::Dialog(QString id, QWidget *parent)
    : QDialog(parent)
    , id(id)
    , closeOnReturn(true)
{
    QSettings settings;
    if (settings.contains(id + "/size"))
        resize(settings.value(id + "/size").toSize());
    if (settings.contains(id + "/position"))
        move(settings.value(id + "/position").toPoint());
}

void Dialog::resizeEvent(QResizeEvent *)
{
    QSettings settings;
    settings.setValue(id + "/size", size());
}

void Dialog::moveEvent(QMoveEvent *)
{
    QSettings settings;
    settings.setValue(id + "/position", pos());
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (closeOnReturn && event->key() == Qt::Key_Return && event->modifiers() | Qt::ControlModifier)
        accept();
    else
        QDialog::keyPressEvent(event);
}
