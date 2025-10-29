#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QLabel>

class Tooltip : public QLabel
{
    Q_OBJECT
    QTimer *timer;

public:
    Tooltip(const QString &text);
    bool hasText(const QString &text) const;
    static void tooltip(const QString &text);
    static void clearTooltip();
    void update();
};

#endif // TOOLTIP_H
