#ifndef TUNING_H
#define TUNING_H

#define UNDO_HISTORY_SIZE             1000
#define MAX_NUM_CONTROLLERS           32 // When parsing a patch



// Standard for all dialogs
#define STANDARD_SPACING              10
#define COLOR_LINE                    QColor(70, 70, 70)
#define COLOR_FILL_CURSOR             QColor(128, 192, 255, 70)
#define COLOR_FRAME_CURSOR            QColor(255, 200, 50)
#define COLOR_TEXT                    QColor(255, 255, 255)

// Rack view
#define MIN_RACK_HEIGHT               200
#define MAX_RACK_HEIGHT               300
#define PIXEL_PER_HP                  88
#define COLOR_RACK_BACKGROUND         QColor(20, 20, 20)

// Patch view
#define CIRCUIT_VERTICAL_MARGIN       (2 * STANDARD_SPACING)
#define CIRCUIT_COMMENT_PADDING       (0.5 * STANDARD_SPACING)
#define COLOR_PATCH_BACKGROUND        QColor(20, 20, 20)
#define COLOR_JACK_INPUT              QColor(0, 200, 255)
#define COLOR_JACK_OUTPUT             QColor(245, 60, 50)
#define COLOR_JACK_UNKNOWN            COLOR_TEXT_UNKNOWN
#define COLOR_OPERATOR_TEXT           QColor(255, 255, 255)
#define COLOR_OPERATOR_MULT           QColor(60, 60, 60)
#define COLOR_OPERATOR_PLUS           QColor(60, 60, 60)
#define COLOR_TEXT_UNKNOWN            QColor(160, 160, 160)
#define COLOR_CIRCUIT_BACKGROUND      QColor(30, 30, 30)
#define COLOR_CIRCUIT_NAME            QColor(250, 250, 250)
#define COLOR_CIRCUIT_NAME_BACKGROUND QColor(60, 60, 60)
#define COLOR_COMMENT                 QColor(200, 200, 200)
#define COLOR_COMMENT_BACKGROUND      QColor(40, 40, 40)

// Circuit chooser (CICH)
#define CICH_DIALOG_WIDTH             890
#define CICH_DIALOG_HEIGHT            500
#define CICH_GLOBAL_MARGIN            STANDARD_SPACING
#define CICH_ICON_MARGIN              STANDARD_SPACING
#define CICH_ICON_WIDTH               50
#define CICH_CIRCUIT_HEIGHT           (CICH_ICON_WIDTH + 2 * (CICH_PADDING))
#define CICH_CIRCUIT_DISTANCE         STANDARD_SPACING
#define CICH_TITLE_HEIGHT             20
#define CICH_PADDING                  (STANDARD_SPACING / 2)
#define CICH_WIDTH_MARGIN             (2 * CICH_GLOBAL_MARGIN)
#define CICH_BACKGROUND_COLOR         QColor(20, 20, 20)
#define CICH_CIRCUIT_BACKGROUND_COLOR QColor(40, 40, 40)
#define CICH_COLOR_TITLE              QColor(255, 255, 255)
#define CICH_COLOR_DESCRIPTION        QColor(190, 190, 190)

// Jack selector (JSEL)
#define JSEL_TOTAL_WIDTH              600
#define JSEL_BACKGROUND_HEIGHT        400
#define JSEL_JACK_HEIGHT              22
#define JSEL_CIRCUIT_TEXT_HEIGHT      22
#define JSEL_JACK_WIDTH               120
#define JSEL_JACK_HORIZONTAL_PADDING  STANDARD_SPACING
#define JSEL_JACK_VERTICAL_PADDING    2
#define JSEL_COLOR_JACK_BACKGROUND    QColor(0, 0, 0)
#define JSEL_COLOR_CIRCUIT_BACKGROUND QColor(20, 20, 20)
#define JSEL_COLOR_JACK_LINE          QColor(80, 80, 80)
#define JSEL_JACK_SPACING             STANDARD_SPACING
#define JSEL_CIRCUIT_WIDTH            150
#define JSEL_CIRCUIT_HEIGHT           200
#define JSEL_CIRCUIT_ICON_WIDTH        80


#endif // TUNING_H
