#ifndef TUNING_H
#define TUNING_H

#define APPLICATION_NAME                     "DROID Forge"
#define SHOP_PRODUCTS_URL                    "https://shop.dermannmitdermaschine.de/products/"
#define PATCH_DIRECTORY_NAME                 "DROID Patches"
#define SECTION_DEFAULT_NAME                 TR("Circuits")
#define CIRCUIT_ICON_PATH                    ":images/circuits/"
#define ICON_PATH_TEMPLATE                   ":images/icons/white/%1.png"
#define CIRCUIT_ICON_SUFFIX                  ".png"
#define FRACTION_PRECISION                   0.0001

// #define NUMBER_INPUTS                        8
// #define NUMBER_OUTPUTS                       8
// #define NUMBER_GATES                         12
// #define NUMBER_RGB_LEDS                      32
// #define NUMBER_EXTRA_REGISTERS               1

#define MAX_LENGTH_SHORTHAND                 5 // for register labels
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
// #define COLOR_LINE                           QColor(120, 120, 120)
// #define COLOR_CURSOR_NORMAL                  QColor(255, 255, 255)
// #define COLOR_CURSOR_PROBLEM                 QColor(255, 0, 0)
// #define COLOR_CURSOR_PATCHING                QColor(50, 100, 255)
// #define COLOR_CURSOR_DISABLED                QColor(160, 160, 160, 128)
// #define COLOR_CURSOR_INACTIVE                QColor(180, 180, 180) // JackSelector
#define COLOR_TEXT                           QColor(255, 255, 255)
#define COLOR_TEXT_UNKNOWN                   QColor(255, 255, 0)
#define COLOR_TEXT_DISABLED                  QColor(128, 128, 128)
// #define COLOR_JACK_INPUT                     QColor(0, 200, 255)
// #define COLOR_JACK_OUTPUT                    QColor(245, 60, 50)
#define COLOR_JACK_UNKNOWN                   COLOR_TEXT_UNKNOWN
#define COLOR_STANDARD_BACKGROUND            QColor(30, 30, 30)
#define COLOR_STATUSBAR_BACKGROUND           QColor(60, 60, 60)
#define BACKGROUND_PIXMAP_HEIGHT             500 // tiling of background image
#define ASSUMED_SCROLLBAR_WIDTH              40

// Patch problem indicator
#define PPI_WIDTH                            130
#define PPI_SIDE_PADDING                     STANDARD_SPACING
#define PPI_IMAGE_MARGIN                     3
#define PPI_BACKGROUND_COLOR                 COLOR_STATUSBAR_BACKGROUND

// Clipboard indicator
#define CI_WIDTH                            130
#define CI_SIDE_PADDING                     STANDARD_SPACING
#define CI_IMAGE_MARGIN                     3
#define CI_BACKGROUND_COLOR                 COLOR_STATUSBAR_BACKGROUND

// Cable status indicator in status bar
#define CSI_IMAGE_MARGIN                     3
#define CSI_WIDTH                            350
#define CSI_MAX_NAME_WIDTH                   150
#define CSI_SIDE_PADDING                     STANDARD_SPACING
#define CSI_BACKGROUND_COLOR                 COLOR_STATUSBAR_BACKGROUND
#define CSI_CABLE_THICKNESS                  0.4
#define CSI_CABLE_COLOR                      QColor(30, 30, 30)
#define CSI_CABLE_HILITE_COLOR               QColor(80, 80, 80)
#define CSI_LABEL_BACKGROUND                 QColor(0, 0, 0, 90)
#define CSI_LABEL_FONT_SIZE                  12
#define CSI_MARKER_WIDTH                     20
#define CSI_MARKER_DISTANCE                  STANDARD_SPACING
#define CSI_MARKER_FONT_SIZE                 9
#define CSI_MARKER_LETTER_SPACING            95
#define CSI_GOOD_MARKER_BACKGROUND           QColor(50, 50, 50)
#define CSI_GOOD_MARKER_BORDER               QColor(180, 180, 180)
#define CSI_BAD_MARKER_BACKGROUND            QColor(180, 0, 0)
#define CSI_BAD_MARKER_BORDER                QColor(255, 0, 0)
#define CSI_NAME_PADDING                     STANDARD_SPACING
#define CSI_ANIMATION_RANGE                  20
#define CSI_ANIMATION_DURATION               1000

// Rack view
#define RACV_SPLITTER_HANDLE_WIDTH           12
#define RACV_MIN_HEIGHT                      100
#define RACV_MAX_HEIGHT                      400
#define RACV_PIXEL_PER_HP                    87
#define RACV_CONTROLLER_GAP                  (2 * RACV_PIXEL_PER_HP)
// #define RACV_BACKGROUND_HEIGHT               2510
#define RACV_MAIN_MARGIN                     30
#define RACV_MODULE_HEIGHT                   (12.8 * RACV_PIXEL_PER_HP / 0.508)
#define RACV_REGMARKER_EXTRA_DIAMETER        30
#define RACV_REGMARKER_PEN_WIDTH             10
#define RACV_REGMARKER_PEN_STYLE             Qt::DotLine
// #define RACV_REGMARKER_BACKGROUND            QColor(255, 255, 255, 100)
// #define RACV_REGMARKER_PEN_COLOR             QColor(255, 255, 255)
// #define RACV_REGHILITES_PEN_COLOR            QColor(255, 128, 0)
// #define RACV_REGHILITES_BACKGROUND           QColor(255, 128, 0, 128)

#define CONTROL_LARGE_POT_SIZE               4.1
#define CONTROL_SMALL_POT_SIZE               1.4
#define CONTROL_BUTTON_SIZE                  2.7
#define CONTROL_SMALL_SWITCH_SIZE            2.0
#define CONTROL_M4_FADER_SIZE                3.0
#define CONTROL_M4_TOUCH_SIZE                2.2
#define CONTROL_RGBLED_SIZE                  1.6
#define CONTROL_JACK_SIZE                    2.5
#define CONTROL_EXTRA_SIZE                   7.8

// Rack label dialog
#define RLD_MINIMUM_LABEL_WIDTH              150

// Controller Selector
#define CSEL_HEIGHT                          2600
#define CSEL_SIDE_MARGIN                     50
#define CSEL_TOP_MARGIN                      150
#define CSEL_PIXEL_PER_HP                    RACV_PIXEL_PER_HP
#define CSEL_CONTROLLER_DISTANCE             200
#define CSEL_CURSOR_WIDTH                    8
#define CSEL_CURSOR_HEIGHT                   2300

// Patch section manager
#define PSM_COLOR_BACKGROUND                 COLOR_STANDARD_BACKGROUND
#define PSM_COLOR_TITLE                      QColor(80, 80, 80)
#define PSM_SIDE_PADDING                     STANDARD_SPACING
#define PSM_TOP_PADDING                      (STANDARD_SPACING / 2)
#define PSM_VERTICAL_DISATNCE                (STANDARD_SPACING / 2)
#define PSM_SECTION_HEIGHT                   20
// #define PSM_COLOR_SECTION_BACKGROUND         QColor(40, 40, 40)
#define PSM_TEXT_PADDING                     (STANDARD_SPACING / 2)
#define PSM_IMAGE_MARGIN                     4

// Patch section view
#define CURSOR_VISIBILITY_MARGIN             30
#define PSV_SCROLL_SPEED                     0.2

// Circuit view (in section view)
#define CIRV_TOP_PADDING                     (2 * STANDARD_SPACING)
#define CIRV_BOTTOM_PADDING                  CIRV_TOP_PADDING
#define CIRV_FOLDED_PADDING                  (STANDARD_SPACING / 2)
#define CIRV_FOLDING_COMMENT_INDENT          200
#define CIRV_SIDE_PADDING                    (2 * STANDARD_SPACING)
#define CIRV_COMMENT_PADDING                 (0.5 * STANDARD_SPACING)
#define CIRV_TEXT_SIDE_PADDING               7
#define CIRV_COMMENT_LINE_HEIGHT             18
#define CIRV_JACK_HEIGHT                     24
#define CIRV_ICON_WIDTH                      15
#define CIRV_ICON_MARGIN                     (STANDARD_SPACING / 2)
#define CIRV_HEADER_HEIGHT                   22
#define CIRV_COLUMN_JACK_MINIMUM_WIDTH       110
#define CIRV_COLUMN_ATOM_MINIMUM_WIDTH       80
#define CIRV_COLUMN_OPERATOR_WIDTH           20
// #define CIRV_COLOR_LINE                      QColor(0, 0, 0, 96)
// #define CIRV_COLOR_OPERATOR                  QColor(255, 255, 255)
// #define CIRV_COLOR_OPERATOR_BACKGROUND       QColor(60, 60, 60)
// #define CIRV_COLOR_BACKGROUND                QColor(40, 40, 40, 160)
#define CIRV_COLOR_CIRCUIT_NAME              QColor(250, 250, 250)
// #define CIRV_COLOR_COMMENT                   QColor(180, 200, 200)
#define CIRV_COLOR_COMMENT_BACKGROUND        QColor(40, 40, 40)
// #define CIRV_COLOR_SELECTION                 QColor(255, 200, 50, 90)

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

// Atom selector
#define ASEL_SUBSELECTOR_WIDTH               150

// Atom selector one liner
#define ASON_X_OFFSET                        13
#define ASON_Y_OFFSET                        46

#endif // TUNING_H
