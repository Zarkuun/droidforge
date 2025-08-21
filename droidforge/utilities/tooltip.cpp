#include <QCursor>
#include <QTimer>

#include "tooltip.h"
#include "tuning.h"

QList<Tooltip*> Tooltip::activeTooltips;

Tooltip::Tooltip(const QString &text, int durationMs)
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
    move(QCursor::pos() + QPoint(0, 20));
    show();

    // Vorherige Tooltips ausblenden
    for (Tooltip* t : activeTooltips)
    {
        t->hide();
        t->deleteLater();
    }
    activeTooltips.clear();

    activeTooltips.append(this);

    // Timer, um Tooltip nach durationMs zu verstecken
    QTimer::singleShot(durationMs, this, [this]() {
        hide();
        deleteLater();
        activeTooltips.removeAll(this);
    });
}

void Tooltip::tooltip(const QString &text)
{
    new Tooltip(text, TOOLTIP_LINGER_TIME_MS);
}
