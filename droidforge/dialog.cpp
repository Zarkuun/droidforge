#include "dialog.h"

#include <QSettings>


Dialog::Dialog(QString id, QWidget *parent)
    : QDialog(parent)
    , id(id)
{
    QSettings settings;
    if (settings.contains(id + "/size"))
        resize(settings.value(id + "/size").toSize());
    if (settings.contains(id + "/position"))
        move(settings.value(id + "/position").toPoint());
}

void Dialog::resizeEvent(QResizeEvent *)
{
    qDebug() << "resize" << id;
    QSettings settings;
    settings.setValue(id + "/size", size());
}

void Dialog::moveEvent(QMoveEvent *)
{
    qDebug() << "move" << id;
    QSettings settings;
    settings.setValue(id + "/position", pos());

}
