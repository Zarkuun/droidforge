#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include "dialog.h"

#include <QListWidgetItem>
#include <QSettings>
#include <QColorDialog>


typedef enum {
    COLOR_LINE = 50,
    CIRV_COLOR_LINE = 51,
    CIRV_COLOR_BACKGROUND = 52,
    COLOR_CURSOR_NORMAL = 100,
    COLOR_CURSOR_PROBLEM = 102,
    COLOR_CURSOR_PATCHING = 103,
    COLOR_CURSOR_DISABLED = 104,
    COLOR_CURSOR_INACTIVE = 105,
    COLOR_JACK_INPUT = 201,
    COLOR_JACK_OUTPUT = 202,
    RACV_REGMARKER_BACKGROUND = 301,
    RACV_REGMARKER_PEN_COLOR = 302,
    RACV_REGHILITES_PEN_COLOR = 303,
    RACV_REGHILITES_BACKGROUND = 304,
    RACV_COLOR_DRAGARROW = 320,
    RACV_COLOR_DRAGARROW_UNLOCKED = 321,
    RACV_COLOR_DRAGARROW_UNSUITABLE = 322,
} color_t;

class ColorScheme : public Dialog
{
    Q_OBJECT
    QSettings settings;
    QColorDialog *colorDialog;
    int currentIndex = -1;

public:
    ColorScheme(QWidget *parent = nullptr);
    QColor color(int index);
    void setColor(int index, const QColor &color);

public slots:
    void itemSelected(QListWidgetItem *item);
    void colorChanged(const QColor &color);

signals:
    void changed();
};

extern ColorScheme *the_colorscheme;

#define COLOR(x) (the_colorscheme->color(x))

#endif // COLORSCHEME_H
