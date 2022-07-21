#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include "dialog.h"

#include <QListWidgetItem>
#include <QSettings>
#include <QColorDialog>


typedef enum {
    COLOR_PATCH_BACKGROUND = 40,
    COLOR_RACK_BACKGROUND = 41,
    CIRV_COLOR_LINE = 51,
    CIRV_COLOR_CIRCUIT_NAME = 83,
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
    CSEL_COLOR_BACKGROUND = 90,
    CSEL_COLOR_CURSOR = 91,
    COLOR_CURSOR_NORMAL = 100,
    COLOR_CURSOR_PROBLEM = 102,
    COLOR_CURSOR_PATCHING = 103,
    COLOR_CURSOR_DISABLED = 104,
    COLOR_CURSOR_INACTIVE = 105,
    RACV_REGMARKER_BACKGROUND = 301,
    RACV_REGMARKER_PEN_COLOR = 302,
    RACV_REGHILITES_PEN_COLOR = 303,
    RACV_REGHILITES_LESSER_PEN_COLOR = 304,
    RACV_COLOR_REGISTER_LABEL = 307,
    RACV_COLOR_LABEL_BG = 308,
    RACV_COLOR_DRAGARROW = 320,
    RACV_COLOR_DRAGARROW_UNLOCKED = 321,
    RACV_COLOR_DRAGARROW_UNSUITABLE = 322,
    JSEL_COLOR_LINE = 400,
    JSEL_COLOR_JACK_BACKGROUND = 401,
    JSEL_COLOR_CIRCUIT_NAME = 404,
    JSEL_COLOR_CIRCUIT_BACKGROUND = 402,
    JSEL_COLOR_JACK_INACTIVE = 403,
    JSEL_COLOR_CURSOR_INACTIVE = 405,
    JSEL_COLOR_DESCRIPTION = 408,
    JSEL_COLOR_DESCRIPTION_BACKGROUND = 409,
    CICH_COLOR_TITLE = 450,
    CICH_RAMSIZE_COLOR = 454,
    CICH_COLOR_DESCRIPTION = 451,
    CICH_COLOR_CIRCUIT_BACKGROUND = 452,
    CICH_COLOR_BACKGROUND = 453,
    PSM_COLOR_SECTION_BACKGROUND = 500,
    PSM_COLOR_TITLE = 501,
    CSI_CABLE_COLOR = 350,
    CSI_CABLE_HILITE_COLOR = 351,
    CSI_BAD_MARKER_BORDER = 352,
    CSI_BAD_MARKER_BACKGROUND = 353,
    CSI_GOOD_MARKER_BORDER = 354,
    CSI_GOOD_MARKER_BACKGROUND = 355,
    CSI_LABEL_BACKGROUND = 356,
    CSI_LABEL_COLOR = 357,
    CSI_MARKER_TEXT_COLOR = 358,
    COLOR_STATUSBAR_BACKGROUND = 550,
    COLOR_STATUSBAR_TEXT = 551,
} color_t;

class ColorScheme : public Dialog
{
    Q_OBJECT

    QSettings settings;
    QColorDialog *colorDialog;
    int currentIndex = -1;
    bool dark;
    QPixmap background;
    QMap<int, QColor> *colors;

public:
    ColorScheme(QWidget *parent = nullptr);
    QColor color(int index);
    void setColor(int index, const QColor &color);
    void hideEvent(QHideEvent *);
    void dumpHeaderFile();
    bool isDevelopment() const;
    bool isDark() const { return dark; };
    QPixmap backgroundPixmap() const { return background; };

private:
    QString settingsKey(int index);

public slots:
    void itemSelected(QListWidgetItem *item);
    void colorChanged(const QColor &color);

signals:
    void changed();
};

extern ColorScheme *the_colorscheme;

#define COLOR(x) (the_colorscheme->color(x))

#endif // COLORSCHEME_H
