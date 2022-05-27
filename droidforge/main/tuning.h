#ifndef TUNING_H
#define TUNING_H

#define APPLICATION_NAME                     "DROID Forge"
#define SHOP_PRODUCTS_URL                    "https://shop.dermannmitdermaschine.de/products/"
#define PATCH_DIRECTORY_NAME                 "DROID Patches"
#define SECTION_DEFAULT_NAME                 "Circuits"
#define CIRCUIT_ICON_PATH                    ":images/circuits/"
#define CIRCUIT_ICON_SUFFIX                  ".png"

#define MAX_RECENT_FILES                     10
#define UNDO_HISTORY_SIZE                    1000
#define MAX_NUM_CONTROLLERS                  32 // When parsing a patch
#define MAX_NUM_SUBJACKS                     64
#define BOOLEAN_VALUE_THRESHOLD              0.1
#define ZOOM_STEP                            1.16 // 10% per Ctrl-+
#define ZOOM_MIN                             -5
#define ZOOM_MAX                             6

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

// Cable status indicator in status bar
#define CSD_IMAGE_MARGIN                     3
#define CSD_WIDTH                            300
#define CSD_SIDE_PADDING                     STANDARD_SPACING
#define CSD_BACKGROUND_COLOR                 QColor(160, 160, 160)
#define CSD_CABLE_THICKNESS                  0.4

// Rack view
#define RACV_SPLITTER_HANDLE_WIDTH           12
#define RACV_MIN_HEIGHT                      100
#define RACV_MAX_HEIGHT                      400
#define RACV_PIXEL_PER_HP                    87
#define RACV_BACKGROUND_HEIGHT               2510
#define RACV_TOP_MARGIN                      170
#define RACV_MODULE_HEIGHT                   (12.8 * RACV_PIXEL_PER_HP / 0.508)
#define RACV_REGMARKER_EXTRA_DIAMETER        30
#define RACV_REGMARKER_BACKGROUND            QColor(255, 255, 255, 100)
#define RACV_REGMARKER_PEN_COLOR             QColor(255, 255, 255)
#define RACV_REGMARKER_PEN_WIDTH             10
#define RACV_REGMARKER_PEN_STYLE             Qt::DotLine

#define CONTROL_LARGE_POT_SIZE               4.1
#define CONTROL_SMALL_POT_SIZE               1.4
#define CONTROL_BUTTON_SIZE                  2.0
#define CONTROL_SMALL_SWITCH_SIZE            1.5
#define CONTROL_M4_FADER_SIZE                3.0
#define CONTROL_M4_TOUCH_SIZE                2.2
#define CONTROL_RGBLED_SIZE                  1.6
#define CONTROL_JACK_SIZE                    1.8
#define CONTROL_EXTRA_SIZE                   7.8

// Controller Selector
#define CSEL_HEIGHT                          2600
#define CSEL_SIDE_MARGIN                     50
#define CSEL_TOP_MARGIN                      150
#define CSEL_PIXEL_PER_HP                    RACV_PIXEL_PER_HP
#define CSEL_CONTROLLER_DISTANCE             200
#define CSEL_CURSOR_WIDTH                    8
#define CSEL_CURSOR_HEIGHT                   2300

// Circuit view (in patch view)
#define CIRV_TOP_PADDING                     (2 * STANDARD_SPACING)
#define CIRV_BOTTOM_PADDING                  CIRV_TOP_PADDING
#define CIRV_SIDE_PADDING                    (2 * STANDARD_SPACING)
#define CIRV_COMMENT_PADDING                 (0.5 * STANDARD_SPACING)
#define CIRV_TEXT_SIDE_PADDING               7
#define CIRV_COMMENT_LINE_HEIGHT             18
#define CIRV_JACK_HEIGHT                     22
#define CIRV_ICON_WIDTH                      15
#define CIRV_ICON_MARGIN                     (STANDARD_SPACING / 2)
#define CIRV_HEADER_HEIGHT                   22
#define CIRV_COLUMN_JACK_MINIMUM_WIDTH       110
#define CIRV_COLUMN_ATOM_MINIMUM_WIDTH       80
#define CIRV_ASSUMED_SCROLLBAR_WIDTH         40
#define CIRV_COLUMN_OPERATOR_WIDTH           20
#define CIRV_COLOR_LINE                      QColor(0, 0, 0, 96)
#define CIRV_COLOR_OPERATOR                  QColor(255, 255, 255)
#define CIRV_COLOR_OPERATOR_BACKGROUND       QColor(60, 60, 60)
#define CIRV_COLOR_CIRCUIT_BACKGROUND        QColor(30, 37, 45, 128)
#define CIRV_COLOR_CIRCUIT_NAME              QColor(250, 250, 250)
#define CIRV_COLOR_CIRCUIT_NAME_BACKGROUND   QColor(60, 60, 60, 128)
#define CIRV_COLOR_COMMENT                   QColor(200, 200, 200)
#define CIRV_COLOR_COMMENT_BACKGROUND        QColor(40, 40, 40)
#define CIRV_COLOR_SELECTION                 QColor(255, 200, 50, 90)

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
