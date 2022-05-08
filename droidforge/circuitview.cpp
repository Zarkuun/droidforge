#include "circuitview.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "tuning.h"

#include <QPainter>

#define SIDE_PADDING    5
#define COMMENT_LINE_HEIGHT 18
#define JACK_HEIGHT    18
#define HEADER_HEIGHT  20

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


CircuitView::CircuitView(Circuit *circuit, unsigned lineHeight)
    : circuit(circuit)
    , lineHeight(lineHeight)
    , selected(false)
    , currentJack(-2)
    , currentColumn(0)
{
    effect.setBlurRadius(15);
    effect.setColor(QColor(0,0,0));
    effect.setOffset(0, 0);
    effect.setEnabled(false);
    setGraphicsEffect(&effect);
}

unsigned CircuitView::commentHeight() const
{
    if (circuit->hasComment())
        return circuit->numCommentLines() * lineHeight
                + CIRCUIT_COMMENT_PADDING * 2;
    else
        return 0;
}


unsigned CircuitView::contentHeight() const
{
    unsigned num_jacks = circuit->numJackAssignments();
    return HEADER_HEIGHT
         + commentHeight()
         + num_jacks * JACK_HEIGHT;
}


QRectF CircuitView::boundingRect() const
{
    unsigned height = contentHeight() + CIRCUIT_VERTICAL_MARGIN;
    return QRectF(-SIDE_PADDING, -CIRCUIT_VERTICAL_MARGIN / 2, WIDTH, height);
}


void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    unsigned height = contentHeight();
    painter->fillRect(0, 0, WIDTH, height, COLOR_CIRCUIT_BACKGROUND);

    unsigned x = SIDE_PADDING;
    unsigned y = 0;

    // Circuit name
    QRect r(0, y, WIDTH, HEADER_HEIGHT);
    painter->fillRect(r, COLOR_CIRCUIT_NAME_BACKGROUND);
    if (selected && currentJack == -2)
        painter->fillRect(r, COLOR_FILL_CURSOR);
    painter->setPen(COLOR_CIRCUIT_NAME);
    painter->drawText(QRect(x, y, WIDTH-x, HEADER_HEIGHT),
                      Qt::AlignVCenter, circuit->getName().toUpper());
    y += HEADER_HEIGHT;

    // Comment
    r = QRect(0, y, WIDTH, commentHeight());
    painter->fillRect(r, COLOR_COMMENT_BACKGROUND);
    if (selected && currentJack == -1)
        painter->fillRect(r, COLOR_FILL_CURSOR);
    painter->setPen(COLOR_COMMENT);
    painter->save();
    painter->drawText(QRect(SIDE_PADDING, y + CIRCUIT_COMMENT_PADDING, WIDTH - 2*SIDE_PADDING, commentHeight()),
                      Qt::AlignLeft | Qt::AlignJustify | Qt::AlignTop, circuit->getComment());
    painter->restore();
    y += commentHeight();


    unsigned line = 0;
    paintJacks(painter, line, y);

    unsigned t = HEADER_HEIGHT + commentHeight();
    painter->save();
    painter->setPen(COLOR_LINE);
    painter->drawLine(0, 0, 0, height);
    painter->drawLine(COLUMN_1_X, t, COLUMN_1_X, height);
    painter->drawLine(WIDTH, 0, WIDTH, height);
    painter->drawLine(0, 0, WIDTH, 0);
    painter->drawLine(0, height, WIDTH, height);
    painter->restore();
}

QStringList CircuitView::usedJacks() const
{
    QStringList used;
    for (qsizetype i=0; i<circuit->numJackAssignments(); i++) {
        JackAssignment *ja = circuit->jackAssignment(i);
        used.append(ja->jackName());
    }
    return used;
}


void CircuitView::paintJacks(QPainter *painter, unsigned &line, unsigned y)
{
    painter->save();
    for (qsizetype i=0; i<circuit->numJackAssignments(); i++) {
        JackAssignment *ja = circuit->jackAssignment(i);
        QColor textcolor;
        switch (ja->jackType()) {
        case JACKTYPE_INPUT: textcolor = COLOR_JACK_INPUT; break;
        case JACKTYPE_OUTPUT: textcolor = COLOR_JACK_OUTPUT; break;
        default: textcolor = COLOR_JACK_UNKNOWN; break;
        }
        paintJack(painter, ja, textcolor, y, selected && (int)line == currentJack);
        y += JACK_HEIGHT;
        line++;
    }
    painter->restore();
}


void CircuitView::paintJack(QPainter *painter, JackAssignment *ja, const QColor textcolor, unsigned y, bool sel)
{
    // Column 0: Name of the jack.
    painter->setPen(textcolor);
    painter->drawText(QRect(COLUMN_0_X + SIDE_PADDING, y, COLUMN_0_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, ja->jackName());
    painter->setPen(COLOR_LINE);
    painter->drawLine(0, y, WIDTH, y);

    // Column 1: A (first mult). Only for inputs
    if (ja->jackType() == JACKTYPE_INPUT)
    {
        JackAssignmentInput *jai = (JackAssignmentInput *)ja;
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
                painter->fillRect(l, y, w, JACK_HEIGHT, COLOR_FILL_CURSOR);
        }
        Atom *atom = jai->getAtom(0);
        if (atom) {
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(COLUMN_1_X + SIDE_PADDING, y, COLUMN_1_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        atom = jai->getAtom(1);
        if (atom) {
            QString value = atom->toString();
            painter->setPen(COLOR_TEXT);
            painter->drawText(QRect(COLUMN_2_X + SIDE_PADDING, y, COLUMN_2_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, value);
        }
        atom = jai->getAtom(2);
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

    else {
        QString text;
        if (ja->jackType() == JACKTYPE_OUTPUT)
        {
            JackAssignmentOutput *jao = (JackAssignmentOutput *)ja;
            Atom *atom = jao->getAtom();

            if (atom) {
                text = atom->toString();
                painter->setPen(COLOR_TEXT);
            }
        }
        else  // UNKNOWN
        {
            JackAssignmentUnknown *jau = (JackAssignmentUnknown *)ja;
            text = jau->valueToString();
            painter->setPen(COLOR_TEXT_UNKNOWN);
        }
        painter->drawText(QRect(COLUMN_1_X + SIDE_PADDING, y, COLUMN_123_WIDTH - SIDE_PADDING, JACK_HEIGHT), Qt::AlignVCenter, text);
        if (sel && (currentColumn == 1 || currentColumn == 2 || currentColumn == 3))
            painter->fillRect(COLUMN_1_X, y, COLUMN_123_WIDTH, JACK_HEIGHT, COLOR_FILL_CURSOR);
    }

    if (sel && currentColumn == 0)
        painter->fillRect(0, y, COLUMN_1_X, JACK_HEIGHT, COLOR_FILL_CURSOR);
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


void CircuitView::select(const CursorPosition &cursor)
{
    currentJack = cursor.row;
    currentColumn = cursor.column;
    selected = true;
    update();
}


void CircuitView::deselect()
{
    selected = false;
    update();
}

int CircuitView::columnAt(unsigned x)
{
    if (x <= COLUMN_1_X)
        return 0;
    else if (x <= OPERATOR_1_X)
        return 1;
    else if (x <= OPERATOR_2_X)
        return 2;
    else
        return 3;
}


int CircuitView::jackAt(unsigned y)
{
    if (y < HEADER_HEIGHT)
        return -2;
    y -= HEADER_HEIGHT;

    if (y < commentHeight())
        return -1;
    y -= commentHeight();

    int jack = y / JACK_HEIGHT;
    if (jack >= circuit->numJackAssignments())
        return circuit->numJackAssignments() - 1;
    else
        return jack;
}
