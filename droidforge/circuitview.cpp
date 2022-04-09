#include "circuitview.h"
#include "tuning.h"

#include <QPainter>

#define SIDE_PADDING  10
#define JACK_HEIGHT   18
#define HEADER_HEIGHT 20
#define LINE_WIDTH     1
#define WIDTH        600
#define COLUMN_1     200

CircuitView::CircuitView(Circuit *circuit)
    : circuit(circuit)
{
    effect.setBlurRadius(15);
    effect.setColor(QColor(0,0,0));
    effect.setOffset(0, 0);
    effect.setEnabled(false);
    setGraphicsEffect(&effect);
}


QRectF CircuitView::boundingRect() const
{
    unsigned num_jacks = circuit->numJackAssignments();
    unsigned height = HEADER_HEIGHT + JACK_HEIGHT * num_jacks + LINE_WIDTH * (num_jacks + 2);
    return QRectF(0, 0, WIDTH, height);
}


void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QLinearGradient linearGrad(QPointF(100, 100), QPointF(200, 200));
    linearGrad.setColorAt(0, QColor(20, 50, 80));
    linearGrad.setColorAt(0.5, QColor(25, 60, 90));
    linearGrad.setColorAt(1, QColor(20, 50, 80));

    unsigned height = (LINE_WIDTH + JACK_HEIGHT) * circuit->numJackAssignments()
                    + HEADER_HEIGHT
                    + 2 * LINE_WIDTH;
    painter->fillRect(0, 0, WIDTH, height, COLOR_CIRCUIT_BACKGROUND);

    unsigned x = SIDE_PADDING + LINE_WIDTH;
    unsigned y = LINE_WIDTH;
    if (selected)
        painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), QColor(0, 255, 0));
    else
        painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), linearGrad);
    painter->setPen(QColor(COLOR_CIRCUIT_NAME));
    painter->drawText(QRect(x, y, WIDTH-x, HEADER_HEIGHT), Qt::AlignVCenter, circuit->name.toUpper());
    y += LINE_WIDTH + HEADER_HEIGHT;

    y = paintJacks(painter, jacktype_t::JACKTYPE_INPUT, COLOR_JACK_INPUT, y);
    y = paintJacks(painter, jacktype_t::JACKTYPE_OUTPUT, COLOR_JACK_OUTPUT, y);
    y = paintJacks(painter, jacktype_t::JACKTYPE_UNKNOWN, COLOR_JACK_UNKNOWN,  y);


    unsigned t = 2 * LINE_WIDTH + HEADER_HEIGHT;
    painter->save();
    painter->setPen(COLOR_LINE);
    painter->drawLine(0, 0, 0, height);
    painter->drawLine(COLUMN_1, t, COLUMN_1, height);
    painter->drawLine(WIDTH, 0, WIDTH, height);
    painter->drawLine(0, 0, WIDTH, 0);
    painter->drawLine(0, height, WIDTH, height);
    painter->restore();
}


unsigned CircuitView::paintJacks(QPainter *painter, jacktype_t jacktype, const QColor &textcolor, unsigned y)
{
    painter->save();
    unsigned x = SIDE_PADDING + LINE_WIDTH;
    for (qsizetype i=0; i<circuit->numJackAssignments(); i++) {
        JackAssignment *ja = circuit->jackAssignment(i);
        if (ja->jackType == jacktype) {
            painter->setPen(textcolor);
            painter->drawText(QRect(x, y, WIDTH-x, JACK_HEIGHT), Qt::AlignVCenter, ja->jack);
            painter->setPen(COLOR_LINE);
            painter->drawLine(0, y, WIDTH, y);
            // painter->drawRoundedRect(QRect(x, y, WIDTH-x, JACK_HEIGHT), 5, 5);
            y += LINE_WIDTH + JACK_HEIGHT;
        }
    }
    painter->restore();
    return y;
}

void CircuitView::select()
{
    selected = true;
}

void CircuitView::deselect()
{
    selected = false;
}
