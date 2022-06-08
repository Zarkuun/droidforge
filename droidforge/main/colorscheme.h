#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include "dialog.h"

#include <QListWidgetItem>
#include <QSettings>
#include <QColorDialog>


typedef enum {
    COLOR_PATCH_BACKGROUND = 40,
    COLOR_RACK_BACKGROUND = 41,
    COLOR_LINE = 50,
    CIRV_COLOR_LINE = 51,
    CIRV_COLOR_CIRCUIT_NAME_BG = 52,
    CIRV_COLOR_COMMENT_BACKGROUND = 53,
    CIRV_COLOR_INPUT_JACK_BG = 54,
    CIRV_COLOR_OUTPUT_JACK_BG = 55,
    CIRV_COLOR_UNKNOWN_JACK_BG = 56,
    CIRV_COLOR_DISABLED_JACK_BG = 57,
    CIRV_COLOR_ODD_ROW = 81,
    CIRV_COLOR_EVEN_ROW = 82,
    CIRV_COLOR_TEXT = 60,
    CIRV_COLOR_SELECTION = 61,
    CIRV_COLOR_DISABLED_TEXT = 62,
    CIRV_COLOR_COMMENT = 63,
    CIRV_COLOR_INPUT_JACK = 201,
    CIRV_COLOR_OUTPUT_JACK = 202,
    CIRV_COLOR_UNKNOWN_JACK = 203,
    CIRV_COLOR_OPERATOR = 70,
    CIRV_COLOR_OPERATOR_BG = 71,
    COLOR_CURSOR_NORMAL = 100,
    COLOR_CURSOR_PROBLEM = 102,
    COLOR_CURSOR_PATCHING = 103,
    COLOR_CURSOR_DISABLED = 104,
    COLOR_CURSOR_INACTIVE = 105,
    RACV_REGMARKER_BACKGROUND = 301,
    RACV_REGMARKER_PEN_COLOR = 302,
    RACV_REGHILITES_PEN_COLOR = 303,
    RACV_REGHILITES_BG = 304,
    RACV_REGHILITES_INACTIVE_BG = 305,
    RACV_COLOR_DRAGARROW = 320,
    RACV_COLOR_DRAGARROW_UNLOCKED = 321,
    RACV_COLOR_DRAGARROW_UNSUITABLE = 322,
    JSEL_COLOR_LINE = 400,
    PSM_COLOR_SECTION_BACKGROUND = 500,
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
    void hideEvent(QHideEvent *);

public slots:
    void itemSelected(QListWidgetItem *item);
    void colorChanged(const QColor &color);

signals:
    void changed();
};

extern ColorScheme *the_colorscheme;

#define COLOR(x) (the_colorscheme->color(x))

#endif // COLORSCHEME_H
