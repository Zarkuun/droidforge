#ifndef TUNING_H
#define TUNING_H

#define MAX_RECENT_FILES                     10
#define UNDO_HISTORY_SIZE                    1000
#define MAX_NUM_CONTROLLERS                  32 // When parsing a patch
#define MAX_NUM_SUBJACKS                     64
#define BOOLEAN_VALUE_THRESHOLD              0.1
#define SECTION_DEFAULT_NAME                 tr("Circuits")

// Standard for all dialogs
#define STANDARD_SPACING                     10
#define COLOR_LINE                           QColor(120, 120, 120)
#define COLOR_FRAME_CURSOR                   QColor(255, 200, 50)
#define COLOR_TEXT                           QColor(255, 255, 255)
#define COLOR_TEXT_UNKNOWN                   QColor(255, 255, 0)
#define COLOR_JACK_INPUT                     QColor(0, 200, 255)
#define COLOR_JACK_OUTPUT                    QColor(245, 60, 50)
#define COLOR_JACK_UNKNOWN                   COLOR_TEXT_UNKNOWN
#define BACKGROUND_PIXMAP_HEIGHT             500 // tiling of background image

// Rack view
#define SPLITTER_HANDLE_WIDTH                12
#define MIN_RACK_HEIGHT                      100
#define MAX_RACK_HEIGHT                      400
#define PIXEL_PER_HP                         88
#define COLOR_RACK_BACKGROUND                QColor(20, 20, 20)

// Patch view
#define PATV_CIRCUIT_VERTICAL_MARGIN         STANDARD_SPACING
#define PATV_CIRCUIT_COMMENT_PADDING         (0.5 * STANDARD_SPACING)
#define PATV_COLOR_BACKGROUND                QColor(20, 20, 20)

// Circuit view (in patch view)
#define CIRV_SIDE_PADDING                     STANDARD_SPACING
#define CIRV_TEXT_SIDE_PADDING                 7
#define CIRV_COMMENT_LINE_HEIGHT              18
#define CIRV_JACK_HEIGHT                      22
#define CIRV_ICON_WIDTH                       15
#define CIRV_HEADER_HEIGHT                    22
#define CIRV_COLUMN_JACK_WIDTH               180
#define CIRV_COLUMN_ATOM_WIDTH               170
#define CIRV_COLUMN_OPERATOR_WIDTH            20

#define CIRV_COLOR_LINE                      QColor(0, 0, 0)
#define CIRV_COLOR_OPERATOR                  QColor(255, 255, 255)
#define CIRV_COLOR_OPERATOR_BACKGROUND       QColor(60, 60, 60)
#define CIRV_COLOR_CIRCUIT_BACKGROUND        QColor(30, 30, 30)
#define CIRV_COLOR_CIRCUIT_NAME              QColor(250, 250, 250)
#define CIRV_COLOR_CIRCUIT_NAME_BACKGROUND   QColor(60, 60, 60)
#define CIRV_COLOR_COMMENT                   QColor(200, 200, 200)
#define CIRV_COLOR_COMMENT_BACKGROUND        QColor(40, 40, 40)

// Circuit chooser (CICH)
#define CICH_DIALOG_WIDTH                    890
#define CICH_DIALOG_HEIGHT                   500
#define CICH_GLOBAL_MARGIN                   STANDARD_SPACING
#define CICH_ICON_MARGIN                     STANDARD_SPACING
#define CICH_ICON_WIDTH                      50
#define CICH_CIRCUIT_HEIGHT                  (CICH_ICON_WIDTH + 2 * (CICH_PADDING))
#define CICH_CIRCUIT_DISTANCE                STANDARD_SPACING
#define CICH_TITLE_HEIGHT                    20
#define CICH_PADDING                         (STANDARD_SPACING / 2)
#define CICH_WIDTH_MARGIN                    (2 * CICH_GLOBAL_MARGIN)
#define CICH_BACKGROUND_COLOR                QColor(20, 20, 20)
#define CICH_CIRCUIT_BACKGROUND_COLOR        QColor(40, 40, 40)
#define CICH_COLOR_TITLE                     QColor(255, 255, 255)
#define CICH_COLOR_DESCRIPTION               QColor(190, 190, 190)

// Jack selector (JSEL)
#define JSEL_TOTAL_WIDTH                     600
#define JSEL_JACK_HEIGHT                     22
#define JSEL_CIRCUIT_TEXT_HEIGHT             22
#define JSEL_JACK_WIDTH                      120
#define JSEL_JACK_HORIZONTAL_PADDING         STANDARD_SPACING
#define JSEL_JACK_VERTICAL_PADDING           2
#define JSEL_JACK_SPACING                    STANDARD_SPACING
#define JSEL_CIRCUIT_WIDTH                   150
#define JSEL_CIRCUIT_HEIGHT                  200
#define JSEL_CIRCUIT_ICON_WIDTH              80
#define JSEL_SCROLL_MARGIN                   50
#define JSEL_COLOR_JACK_BACKGROUND           QColor(0, 0, 0)
#define JSEL_COLOR_CIRCUIT_BACKGROUND        QColor(20, 20, 20)
#define JSEL_COLOR_JACK_LINE                 QColor(120, 120, 120)
#define JSEL_COLOR_JACK_INACTIVE             QColor(80, 80, 80)
#define JSEL_COLOR_CURSOR_INACTIVE           QColor(250, 250, 250)

// Atom selector
#define ASEL_SUBSELECTOR_WIDTH               150

// Atom selector one liner
#define ASON_X_OFFSET                        13
#define ASON_Y_OFFSET                        46

#endif // TUNING_H
