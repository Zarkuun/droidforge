#include "circuitview.h"
#include "tuning.h"

#include <QPainter>

#define SIDE_PADDING   10
#define JACK_HEIGHT    18
#define HEADER_HEIGHT  20
#define LINE_WIDTH      1

#define COLUMN_JACK_WIDTH    180
#define COLUMN_ATOM_WIDTH    170
#define COLUMN_OPERATOR_WIDTH 20

#define COLUMN_0_WIDTH    COLUMN_JACK_WIDTH
#define COLUMN_1_WIDTH    COLUMN_ATOM_WIDTH
#define COLUMN_2_WIDTH    COLUMN_ATOM_WIDTH
#define COLUMN_3_WIDTH    COLUMN_ATOM_WIDTH

#define COLUMN_0_X    0
#define COLUMN_1_X    COLUMN_JACK_WIDTH
#define OPERATOR_1_X  (COLUMN_1_X + COLUMN_1_WIDTH)
#define COLUMN_2_X    (OPERATOR_1_X + COLUMN_OPERATOR_WIDTH)
#define OPERATOR_2_X  (COLUMN_2_X + COLUMN_2_WIDTH)
#define COLUMN_3_X    (OPERATOR_2_X + COLUMN_OPERATOR_WIDTH)
#define WIDTH         (COLUMN_3_X + COLUMN_3_WIDTH)

#define COLUMN_123_WIDTH (COLUMN_1_WIDTH + COLUMN_2_WIDTH + COLUMN_3_WIDTH + 2 * COLUMN_OPERATOR_WIDTH)


CircuitView::CircuitView(Circuit *circuit)
    : circuit(circuit)
{
    effect.setBlurRadius(15);
    effect.setColor(QColor(0,0,0));
    effect.setOffset(0, 0);
    effect.setEnabled(false);
    setGraphicsEffect(&effect);
}


unsigned CircuitView::contentHeight() const
{
    unsigned num_jacks = circuit->numJackAssignments();
    return HEADER_HEIGHT + JACK_HEIGHT * num_jacks + LINE_WIDTH * (num_jacks + 2);
}


QRectF CircuitView::boundingRect() const
{
    unsigned height = contentHeight() + CIRCUIT_VERTICAL_MARGIN;
    return QRectF(0, -CIRCUIT_VERTICAL_MARGIN / 2, WIDTH, height);
}


void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    unsigned height = contentHeight();
    painter->fillRect(0, 0, WIDTH, height, COLOR_CIRCUIT_BACKGROUND);

    unsigned x = SIDE_PADDING + LINE_WIDTH;
    unsigned y = LINE_WIDTH;
    painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), COLOR_CIRCUIT_NAME_BACKGROUND);
    if (selected && currentJack == -1)
        painter->fillRect(QRect(0, y, WIDTH, HEADER_HEIGHT), COLOR_CURSOR);

    painter->setPen(COLOR_CIRCUIT_NAME);
    painter->drawText(QRect(x, y, WIDTH-x, HEADER_HEIGHT), Qt::AlignVCenter, circuit->name.toUpper());
    y += LINE_WIDTH + HEADER_HEIGHT;

    unsigned line = 0;
    paintJacks(painter, line, y);

    unsigned t = 2 * LINE_WIDTH + HEADER_HEIGHT;
    painter->save();
    painter->setPen(COLOR_LINE);
    painter->drawLine(0, 0, 0, height);
    painter->drawLine(COLUMN_1_X, t, COLUMN_1_X, height);
    painter->drawLine(WIDTH, 0, WIDTH, height);
    painter->drawLine(0, 0, WIDTH, 0);
    painter->drawLine(0, height, WIDTH, height);
    painter->restore();
}


void CircuitView::paintJacks(QPainter *painter, unsigned &line, unsigned y)
{
    painter->save();
    for (qsizetype i=0; i<circuit->numJackAssignments(); i++) {
        JackAssignment *ja = circuit->jackAssignment(i);
        QColor textcolor;
        switch (ja->jackType) {
        case JACKTYPE_INPUT: textcolor = COLOR_JACK_INPUT; break;
        case JACKTYPE_OUTPUT: textcolor = COLOR_JACK_OUTPUT; break;
        default: textcolor = COLOR_JACK_UNKNOWN; break;
        }
        paintJack(painter, ja, textcolor, y, selected && (int)line == currentJack);
        y += LINE_WIDTH + JACK_HEIGHT;
        line++;
    }
    painter->restore();
}


void CircuitView::paintJack(QPainter *painter, JackAssignment *ja, const QColor textcolor, unsigned y, bool sel)
{
    // Column 0: Name of the jack.
    painter->setPen(textcolor);
    painter->drawText(QRect(COLUMN_0_X + SIDE_PADDING, y, COLUMN_0_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, ja->jack);
    painter->setPen(COLOR_LINE);
    painter->drawLine(0, y, WIDTH, y);

    // Column 1: A (first mult). Only for inputs
    if (ja->jackType == JACKTYPE_INPUT)
    {
        if (sel) {
            unsigned l = 0, w = 0;
            if (currentColumn == 1) {
                l = COLUMN_1_X;
                w = COLUMN_1_WIDTH;
            }
            else if (currentColumn == 2) {
                l = COLUMN_2_X;
                w = COLUMN_2_WIDTH;
            }
            else if (currentColumn == 3) {
                l = COLUMN_3_X;
                w = COLUMN_3_WIDTH;
            }
            if (w)
                painter->fillRect(l, y, w, JACK_HEIGHT, COLOR_CURSOR);
        }
        Atom *atom = ja->atomA;
        if (atom) {
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(COLUMN_1_X + SIDE_PADDING, y, COLUMN_1_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        atom = ja->atomB;
        if (atom) {
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(COLUMN_2_X + SIDE_PADDING, y, COLUMN_2_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        atom = ja->atomC;
        if (atom) {
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(COLUMN_3_X + SIDE_PADDING, y, COLUMN_3_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        painter->setPen(COLOR_LINE);
        paintOperator(painter, OPERATOR_1_X, y, "✱", COLOR_OPERATOR_MULT);
        paintOperator(painter, OPERATOR_2_X, y, "+", COLOR_OPERATOR_PLUS);
        painter->setPen(COLOR_LINE);
        painter->drawLine(COLUMN_2_X, y, COLUMN_2_X, y + JACK_HEIGHT);
        painter->drawLine(COLUMN_3_X, y, COLUMN_3_X, y + JACK_HEIGHT);
        painter->drawLine(OPERATOR_1_X, y, OPERATOR_1_X, y + JACK_HEIGHT);
        painter->drawLine(OPERATOR_2_X, y, OPERATOR_2_X, y + JACK_HEIGHT);

        // TODO: B and C
    }

    else { // OUTPUT
        Atom *atom = ja->atomA;
        if (atom) {
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(COLUMN_1_X + SIDE_PADDING, y, COLUMN_123_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        if (sel && (currentColumn == 1 || currentColumn == 2 || currentColumn == 3))
            painter->fillRect(COLUMN_1_X, y, COLUMN_123_WIDTH, JACK_HEIGHT, COLOR_CURSOR);
    }

    if (sel && currentColumn == 0)
        painter->fillRect(0, y, COLUMN_1_X, JACK_HEIGHT, COLOR_CURSOR);
}

void CircuitView::paintOperator(QPainter *painter, unsigned x, unsigned y, QString o, const QColor &color)
{
    QRect r(x, y, COLUMN_OPERATOR_WIDTH, JACK_HEIGHT);
    painter->fillRect(r, color);
    painter->setPen(COLOR_OPERATOR_TEXT);
    painter->save();
    painter->setPen(COLOR_TEXT);
    const QFont &font = painter->font();
    painter->setFont(QFont(font.family(), font.pointSize() * 1.2));
    painter->drawText(r, o, Qt::AlignVCenter | Qt::AlignCenter);
    painter->restore();
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
