#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QLabel>

#include <QList>

class Tooltip : public QLabel
{
    Q_OBJECT

public:
    Tooltip(const QString &text, int durationMs);
    static void tooltip(const QString &text);

private:
    static QList<Tooltip*> activeTooltips;
};

#endif // TOOLTIP_H
