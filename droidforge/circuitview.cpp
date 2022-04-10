#include "circuitview.h"
#include "tuning.h"

#include <QPainter>

#define SIDE_PADDING  10
#define JACK_HEIGHT   18
#define HEADER_HEIGHT 20
#define LINE_WIDTH     1
#define WIDTH        700
#define COLUMN_1     200
#define COLUMN_2     350
#define COLUMN_3     500

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
    if (selected && currentJack == -1)
        painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), QColor(80, 40, 180));
    else
        painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), linearGrad);
    painter->setPen(QColor(COLOR_CIRCUIT_NAME));
    painter->drawText(QRect(x, y, WIDTH-x, HEADER_HEIGHT), Qt::AlignVCenter, circuit->name.toUpper());
    y += LINE_WIDTH + HEADER_HEIGHT;

    unsigned line = 0;
    y = paintJacks(painter, line, jacktype_t::JACKTYPE_INPUT, COLOR_JACK_INPUT, y);
    y = paintJacks(painter, line, jacktype_t::JACKTYPE_OUTPUT, COLOR_JACK_OUTPUT, y);
    y = paintJacks(painter, line, jacktype_t::JACKTYPE_UNKNOWN, COLOR_JACK_UNKNOWN,  y);


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


unsigned CircuitView::paintJacks(QPainter *painter, unsigned &line, jacktype_t jacktype, const QColor &textcolor, unsigned y)
{
    painter->save();
    for (qsizetype i=0; i<circuit->numJackAssignments(); i++) {
        JackAssignment *ja = circuit->jackAssignment(i);
        if (ja->jackType == jacktype) {
            paintJack(painter, ja, textcolor, y, selected && (int)line == currentJack);
            y += LINE_WIDTH + JACK_HEIGHT;
            line++;
        }
    }
    painter->restore();
    return y;
}


void CircuitView::paintJack(QPainter *painter, JackAssignment *ja, const QColor textcolor, unsigned y, bool sel)
{
    if (sel && currentColumn == 0)
        painter->fillRect(0, y, COLUMN_1, JACK_HEIGHT, QColor(50, 70, 80));

    // Column 0: Name of the jack.
    unsigned text_x = SIDE_PADDING + LINE_WIDTH;
    painter->setPen(textcolor);
    painter->drawText(QRect(text_x, y, WIDTH - text_x, JACK_HEIGHT), Qt::AlignVCenter, ja->jack);
    painter->setPen(COLOR_LINE);
    painter->drawLine(0, y, WIDTH, y);

    // Column 1: A (first mult). Only for inputs
    if (ja->jackType == JACKTYPE_INPUT) {
        if (sel) {
            unsigned l = 0, w = 0;
            if (currentColumn == 1) {
                l = COLUMN_1;
                w = COLUMN_2 - COLUMN_1;
            }
            else if (currentColumn == 2) {
                l = COLUMN_2;
                w = COLUMN_3 - COLUMN_2;
            }
            else if (currentColumn == 3) {
                l = COLUMN_3;
                w = WIDTH - COLUMN_2;
            }
            if (w)
                painter->fillRect(l, y, w, JACK_HEIGHT, QColor(50, 70, 80));
        }
        Atom *atom = ja->atomA;
        if (atom) {
            text_x = SIDE_PADDING + COLUMN_1;
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(text_x, y, COLUMN_2 - COLUMN_1, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        atom = ja->atomB;
        if (atom) {
            text_x = SIDE_PADDING + COLUMN_2;
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(text_x, y, COLUMN_3 - COLUMN_2, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        atom = ja->atomC;
        if (atom) {
            text_x = SIDE_PADDING + COLUMN_3;
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(text_x, y, WIDTH - COLUMN_3, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        painter->setPen(COLOR_LINE);
        painter->drawLine(COLUMN_2, y, COLUMN_2, y + JACK_HEIGHT);
        painter->drawLine(COLUMN_3, y, COLUMN_3, y + JACK_HEIGHT);

        // TODO: B and C
    }

    else {
        if (sel && (currentColumn == 1 || currentColumn == 2 || currentColumn == 3))
            painter->fillRect(COLUMN_1, y, WIDTH - COLUMN_1, JACK_HEIGHT, QColor(50, 70, 80));
        Atom *atom = ja->atomA;
        if (atom) {
            text_x = SIDE_PADDING + COLUMN_1;
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(text_x, y, WIDTH - COLUMN_1 - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
    }
}


void CircuitView::select(unsigned cj, unsigned cc)
{
    currentJack = cj;
    currentColumn = cc;
    selected = true;
    update();
}


void CircuitView::deselect()
{
    selected = false;
    update();
}
