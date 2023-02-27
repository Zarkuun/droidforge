#ifndef TUNING_H
#define TUNING_H

#include "os.h"

#define APPLICATION_NAME                     "DROID Forge"
#ifdef QT_DEBUG
#define APPLICATION_VERSION                  ("SNAPSHOT " __DATE__)
#else
#define APPLICATION_VERSION                  "1.1beta1"
#endif
#define LICENSE_URL                          "https://www.gnu.org/licenses/gpl.txt"
#define DISCORD_URL                          "https://discord.com/invite/9TUcRmH"

#define COLOR_DEFINITION_FILE                "/Users/mk/git/droidforge/droidforge/colors_%1.h"
#define DROID_PATCH_FILENAME                 "droid.ini"
#define STATUS_DUMP_FILENAME                 "STATES%1.TXT"
#define MAX_DUMP_FILE_NUMBER                 99 // from Droid:tuning.h
#define SHOP_PRODUCTS_URL                    "https://shop.dermannmitdermaschine.de/products/"
#define PATCH_DIRECTORY_NAME                 "DROID Patches"
#define SECTION_DEFAULT_NAME                 TR("Untitled section")
#define CIRCUIT_ICON_PATH                    ":images/circuits/"
#define ICON_PATH_TEMPLATE                   ":images/icons/%1/%2.png"
#define JACK_TYPE_SYMBOLS_PATH_TEMPLATE      ":images/jacktypes/%1/%2.png"
#define CIRCUIT_ICON_SUFFIX                  ".png"
#define FRACTION_PRECISION                   0.0001
#define NUMBER_DIGITS                        15
#define MAC_UMOUNT_TIMEOUT_MS                25000 // ms
#define SD_CHECK_INTERVAL                    300  // ms
#define MAX_DROID_INI                        64000 // from x7.h
#define MAX_PATCH_LINE_LENGTH                      63 // from master:tuning.h

#define MAX_LENGTH_SHORTHAND                 12 // for register labels
#define MAX_RECENT_FILES                     10
#define UNDO_HISTORY_SIZE                    1000
#define MAX_NUM_CONTROLLERS                  32 // When parsing a patch
#define MAX_NUM_SUBJACKS                     64
#define BOOLEAN_VALUE_THRESHOLD              0.1
#define ZOOM_STEP                            1.16 // 10% per Ctrl-+
#define ZOOM_MIN                             -5
#define ZOOM_MAX                             6

// Main window
#define MAIN_WIDTH                           1000
#define MAIN_HEIGHT                          1000
#define WINDOW_X_DISPLACEMENT                40
#define WINDOW_Y_DISPLACEMENT                15

// Standard for all dialogs
#define STANDARD_SPACING                     10
#define BACKGROUND_PIXMAP_HEIGHT             500 // tiling of background image
#define ASSUMED_SCROLLBAR_WIDTH              40
#define SPLITTER_HANDLE_WIDTH                 8

// Patch problem indicator
#define PPI_WIDTH                            130
#define PPI_SIDE_PADDING                     STANDARD_SPACING
#define PPI_IMAGE_MARGIN                     3

// Clipboard indicator
#define CI_WIDTH                            130
#define CI_SIDE_PADDING                     STANDARD_SPACING
#define CI_IMAGE_MARGIN                     3

// Cable status indicator in status bar
#define CSI_IMAGE_MARGIN                     3
#define CSI_WIDTH                            350
#define CSI_MAX_NAME_WIDTH                   150
#define CSI_SIDE_PADDING                     STANDARD_SPACING
#define CSI_CABLE_THICKNESS                  0.4
#define CSI_MARKER_WIDTH                     20
#define CSI_MARKER_DISTANCE                  STANDARD_SPACING
#define CSI_MARKER_LETTER_SPACING            95
#define CSI_NAME_PADDING                     STANDARD_SPACING
#define CSI_ANIMATION_RANGE                  20
#define CSI_ANIMATION_DURATION               1000

#ifdef Q_OS_WIN
#define CSI_MARKER_FONT_SIZE                 6
#define CSI_LABEL_FONT_SIZE                  8
#else
#define CSI_MARKER_FONT_SIZE                 9
#define CSI_LABEL_FONT_SIZE                  12
#endif

// Rack view
#define RACV_MIN_HEIGHT                      100
#define RACV_MAX_HEIGHT                      1000
#define RACV_NORMAL_HEIGHT                   280
#define RACV_PIXEL_PER_HP                    87
#define RACV_CONTROLLER_GAP                  0 // (2 * RACV_PIXEL_PER_HP)
#define RACV_PADDING_MARGIN                 STANDARD_SPACING
#define RACK_MODULE_MARGIN                   5
#define RACV_MODULE_HEIGHT                   (12.8 * RACV_PIXEL_PER_HP / 0.508)
#define RACV_REGMARKER_EXTRA_DIAMETER        30
#define RACV_REGMARKER_PEN_WIDTH             10
#define RACV_CONTROLLER_DRAG_WIDTH           12
#define RACV_REGISTER_DRAG_WIDTH             12
#define RACV_CONTROLLER_SNAP_DISTANCE        (1.8 * RACV_PIXEL_PER_HP)
#define RACV_ARROW_HEAD_SIZE                 (0.6 * RACV_PIXEL_PER_HP)
#define RACV_DRAG_CIRCLE_SIZE                (2.5 * RACV_PIXEL_PER_HP)
#define RACV_BUTTON_LABEL_WIDTH              2.43
#define RACV_JACK_LABEL_WIDTH                1.93
#define RACV_POT_LABEL_WIDTH                 4.8
#define RACV_LABEL_HEIGHT                    (0.7 * RACV_PIXEL_PER_HP)
#define RACV_HILIGHT_UPDATE_INTERVAL         100 // ms


#define CONTROL_LARGE_POT_SIZE               4.1
#define CONTROL_SMALL_POT_SIZE               1.5
#define CONTROL_BUTTON_SIZE                  2.7
#define CONTROL_SMALL_SWITCH_SIZE            2.0
#define CONTROL_M4_FADER_SIZE                3.0
#define CONTROL_M4_TOUCH_SIZE                2.2
#define CONTROL_RGBLED_SIZE                  2.1
#define CONTROL_JACK_SIZE                    2.5
#define CONTROL_EXTRA_SIZE                   11.5

// Source code editor
#define SCD_WIDTH                            600
#define SCD_HEIGHT                           900

// Rack label dialog
#define RLD_MINIMUM_LABEL_WIDTH              150

// Controller Selector
#define CSEL_HEIGHT                          2300
#define CSEL_SIDE_MARGIN                     100
#define CSEL_TOP_MARGIN                      100
#define CSEL_PIXEL_PER_HP                    RACV_PIXEL_PER_HP
#define CSEL_CONTROLLER_DISTANCE             200
#define CSEL_CURSOR_WIDTH                    16

// Patch section manager
#define PSM_MINIMUM_WIDTH                    150
#define PSM_MAXIMUM_WIDTH                    400
#define PSM_NORMAL_WIDTH                     200
#define PSM_SIDE_PADDING                     STANDARD_SPACING
#define PSM_TOP_PADDING                      11
#define PSM_VERTICAL_DISTANCE                (STANDARD_SPACING / 2)
#define PSM_SECTION_HEIGHT                   CIRV_HEADER_HEIGHT
#define PSM_TEXT_PADDING                     4
#define PSM_IMAGE_MARGIN                     4
#define PSM_SECTION_DRAG_WIDTH               2
#define PSM_ARROW_HEAD_SIZE                  6
#define PSM_SECTION_SNAP_DISTANCE            ((PSM_SECTION_HEIGHT / 2) - 1)

// Patch section view
#define CURSOR_VISIBILITY_MARGIN             30
#define PSV_SCROLL_SPEED                     0.2
#define PSV_COMMENT_PADDING                  (STANDARD_SPACING / 2)

// Circuit view (in section view)
#define CIRV_TOP_PADDING                     11 // (2 * STANDARD_SPACING)
#define CIRV_BOTTOM_PADDING                  CIRV_TOP_PADDING
#define CIRV_FOLDED_PADDING                  (STANDARD_SPACING / 2)
#define CIRV_FOLDING_COMMENT_INDENT          200
#define CIRV_SIDE_PADDING                    11 // (2 * STANDARD_SPACING)
#define CIRV_COMMENT_PADDING                 (0.5 * STANDARD_SPACING)
#define CIRV_TEXT_SIDE_PADDING               7
#define CIRV_COMMENT_LINE_HEIGHT             18
#define CIRV_JACK_HEIGHT                     22
#define CIRV_ICON_WIDTH                      15
#define CIRV_ICON_MARGIN                     (STANDARD_SPACING / 2)
#define CIRV_HEADER_HEIGHT                   26
#define CIRV_COLUMN_JACK_MINIMUM_WIDTH       110
#define CIRV_COLUMN_ATOM_MINIMUM_WIDTH       80
#define CIRV_COLUMN_OPERATOR_WIDTH           20
#define CIRV_CABLEPLUG_HEIGHT                15
#define CIRV_CABLEPLUG_TOP_MARGING           4

// Circuit chooser (CICH)
#define CICH_DIALOG_WIDTH                    890
#define CICH_DIALOG_HEIGHT                   500
#define CICH_GLOBAL_MARGIN                   STANDARD_SPACING
#define CICH_ICON_MARGIN                     STANDARD_SPACING
#define CICH_ICON_WIDTH                      55
#define CICH_CIRCUIT_HEIGHT                  (CICH_ICON_WIDTH + 2 * (CICH_PADDING))
#define CICH_CIRCUIT_DISTANCE                STANDARD_SPACING
#define CICH_TITLE_HEIGHT                    20
#define CICH_PADDING                         (STANDARD_SPACING / 2)
#define CICH_WIDTH_MARGIN                    (2 * CICH_GLOBAL_MARGIN)

// Jack selector (JSEL)
#define JSEL_TOTAL_WIDTH                     600
#define JSEL_DESCRIPTION_WIDTH               200
#define JSEL_JACK_HEIGHT                     22
#define JSEL_CIRCUIT_TEXT_HEIGHT             22
#define JSEL_JACK_WIDTH                      152
#define JSEL_SYMBOL_WIDTH                    (JSEL_JACK_WIDTH / 4)
#define JSEL_JACK_HORIZONTAL_PADDING         STANDARD_SPACING
#define JSEL_JACK_VERTICAL_PADDING           2
#define JSEL_JACK_SPACING                    STANDARD_SPACING
#define JSEL_CIRCUIT_WIDTH                   150
#define JSEL_CIRCUIT_HEIGHT                  200
#define JSEL_CIRCUIT_ICON_WIDTH              100
#define JSEL_SCROLL_MARGIN                   50
#define JSEL_VERTICAL_PADDING                STANDARD_SPACING

// Atom selector
#define ASEL_SUBSELECTOR_WIDTH               150
#define ASEL_JACKTYPE_WIDTH                  40

// Atom selector one liner
#define ASON_X_OFFSET                        13
#define ASON_Y_OFFSET                        46

#endif // TUNING_H
