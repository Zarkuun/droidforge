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
}
void Dialog::resizeEvent(QResizeEvent *)
{
    QSettings settings;
    settings.setValue(id + "/size", size());
}
void Dialog::keyPressEvent(QKeyEvent *event)
{
    if (closeOnReturn && event->key() == Qt::Key_Return && event->modifiers() | Qt::ControlModifier)
        accept();
    else
        QDialog::keyPressEvent(event);
}
void Dialog::setDefaultSize(QSize s)
{
    QSettings settings;
    if (!settings.contains(id + "/size"))
        resize(s);
}
