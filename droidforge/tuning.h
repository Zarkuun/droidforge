#ifndef TUNING_H
#define TUNING_H

#define UNDO_HISTORY_SIZE             1000


// Dimensions
#define MIN_RACK_HEIGHT               200
#define MAX_RACK_HEIGHT               300
#define CIRCUIT_VERTICAL_MARGIN       20
#define PIXEL_PER_HP                  88

// Main colors
#define COLOR_RACK_BACKGROUND         QColor(20, 20, 20)
#define COLOR_PATCH_BACKGROUND        QColor(20, 20, 20)

// Colors for circuit definitions
#define COLOR_LINE                    QColor(255, 255, 255, 40)
#define COLOR_CURSOR                  QColor(128, 192, 255, 70)
#define COLOR_CIRCUIT_BACKGROUND      QColor(30, 30, 30)
#define COLOR_CIRCUIT_NAME_BACKGROUND QColor(60, 60, 60)
#define COLOR_CIRCUIT_NAME            QColor(250, 250, 250)
#define COLOR_TEXT                    QColor(255, 255, 255)
#define COLOR_TEXT_UNKNOWN            QColor(160, 160, 160)
#define COLOR_JACK_INPUT              QColor(0, 200, 255)
#define COLOR_JACK_OUTPUT             QColor(245, 60, 50)
#define COLOR_JACK_UNKNOWN            COLOR_TEXT_UNKNOWN
#define COLOR_OPERATOR_TEXT           QColor(255, 255, 255)
#define COLOR_OPERATOR_MULT           QColor(60, 60, 60)
#define COLOR_OPERATOR_PLUS           QColor(60, 60, 60)


#endif // TUNING_H
