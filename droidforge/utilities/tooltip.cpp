#include <QCursor>
#include <QTimer>

#include "tooltip.h"
#include "tuning.h"

Tooltip *activeTooltip = 0;

Tooltip::Tooltip(const QString &text)
    : QLabel(text)
{
    setWindowFlags(Qt::ToolTip);
    setStyleSheet(R"(
    QLabel {
        background-color: #ffffe0;
        color: black;
        border: 1px solid gray;
        padding: 2px;
        border-radius: 0px;
        font-size: 12px;
    }
    )");
    update();
    show();

}
void Tooltip::update()
{
    if (timer)
        timer->stop();

    move(QCursor::pos() + QPoint(0, 20));
    // Timer, um Tooltip nach durationMs zu verstecken
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        hide();
    });
    timer->start(TOOLTIP_LINGER_TIME_MS);
}
bool Tooltip::hasText(const QString &t) const
{
    return text() == t;
}
void Tooltip::tooltip(const QString &text)
{
    if (activeTooltip != 0) {
        if (activeTooltip->hasText(text)) {
            activeTooltip->update();
            return;
        }
        clearTooltip();
    }
    activeTooltip = new Tooltip(text);
}
void Tooltip::clearTooltip()
{
    if (activeTooltip != 0) {
        activeTooltip->hide();
        activeTooltip->deleteLater();
        activeTooltip = 0;
    }
}
