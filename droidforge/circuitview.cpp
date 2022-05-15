#include "circuitview.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "tuning.h"

#include <QPainter>

#define NUM_COLUMNS 4

unsigned column_widths[NUM_COLUMNS] = {
  CIRV_COLUMN_JACK_WIDTH,
  CIRV_COLUMN_ATOM_WIDTH,
  CIRV_COLUMN_ATOM_WIDTH,
  CIRV_COLUMN_ATOM_WIDTH,
};


unsigned column_positions[NUM_COLUMNS] = {
  0,
  CIRV_COLUMN_JACK_WIDTH,
  CIRV_COLUMN_JACK_WIDTH + CIRV_COLUMN_ATOM_WIDTH + CIRV_COLUMN_OPERATOR_WIDTH,
  CIRV_COLUMN_JACK_WIDTH + 2 * (CIRV_COLUMN_ATOM_WIDTH + CIRV_COLUMN_OPERATOR_WIDTH),
};


#define CIRV_OPERATOR_1_X  (column_positions[1] + column_widths[1])
#define CIRV_OPERATOR_2_X  (column_positions[2] + column_widths[2])
#define CIRV_WIDTH         (column_positions[3] + column_widths[3])

#define CIRV_COLUMN_123_WIDTH (3 * CIRV_COLUMN_ATOM_WIDTH + 2 * CIRV_COLUMN_OPERATOR_WIDTH)


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
                + PATV_CIRCUIT_COMMENT_PADDING * 2;
    else
        return 0;
}


unsigned CircuitView::contentHeight() const
{
    unsigned num_jacks = circuit->numJackAssignments();
    return CIRV_HEADER_HEIGHT
         + commentHeight()
         + num_jacks * CIRV_JACK_HEIGHT;
}


QRectF CircuitView::boundingRect() const
{
    unsigned height = contentHeight() + PATV_CIRCUIT_VERTICAL_MARGIN;
    return QRectF(
                -CIRV_SIDE_PADDING,
                -PATV_CIRCUIT_VERTICAL_MARGIN,
                CIRV_WIDTH + 2*CIRV_SIDE_PADDING,
                height + 2 * PATV_CIRCUIT_VERTICAL_MARGIN);
}


void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    unsigned height = contentHeight();
    painter->fillRect(0, 0, CIRV_WIDTH, height, CIRV_COLOR_CIRCUIT_BACKGROUND);

    unsigned x = CIRV_SIDE_PADDING;

    // Circuit name
    painter->fillRect(headerRect(), CIRV_COLOR_CIRCUIT_NAME_BACKGROUND);
    painter->setPen(CIRV_COLOR_CIRCUIT_NAME);
    painter->drawText(QRect(x, 0, CIRV_WIDTH-x, CIRV_HEADER_HEIGHT),
                      Qt::AlignVCenter, circuit->getName().toUpper());

    // Comment
    painter->fillRect(commentRect(), CIRV_COLOR_COMMENT_BACKGROUND);
    painter->setPen(CIRV_COLOR_COMMENT);
    painter->save();
    painter->drawText(QRect(CIRV_TEXT_SIDE_PADDING, CIRV_HEADER_HEIGHT + PATV_CIRCUIT_COMMENT_PADDING, CIRV_WIDTH - 2*CIRV_TEXT_SIDE_PADDING, commentHeight()),
                      Qt::AlignLeft | Qt::AlignJustify | Qt::AlignTop, circuit->getComment());
    painter->restore();

    unsigned line = 0;
    paintJacks(painter, line, CIRV_HEADER_HEIGHT + commentHeight());

    unsigned t = CIRV_HEADER_HEIGHT + commentHeight();
    painter->save();
    painter->setPen(CIRV_COLOR_LINE);
    painter->drawLine(0, 0, 0, height);
    painter->drawLine(column_positions[1], t, column_positions[1], height);
    painter->drawLine(CIRV_WIDTH, 0, CIRV_WIDTH, height);
    painter->drawLine(0, 0, CIRV_WIDTH, 0);
    painter->drawLine(0, height, CIRV_WIDTH, height);
    painter->restore();

    if (selected)
        paintCursor(painter);
}

void CircuitView::paintCursor(QPainter *painter) const
{
    painter->setPen(COLOR_FRAME_CURSOR);
    if (currentJack == -2)
        painter->drawRect(headerRect());
    else if (currentJack == -1)
        painter->drawRect(commentRect());
    else if (currentColumn == 0)
        painter->drawRect(jackRect(currentJack));
    else
        painter->drawRect(atomRect(currentJack, currentColumn));
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
        paintJack(painter, ja, textcolor, y);
        y += CIRV_JACK_HEIGHT;
        line++;
    }
    painter->restore();
}


void CircuitView::paintAtom(QPainter *painter, const QRect &rect, Atom *atom)
{
    if (atom) {
        QString text = atom->toString();
        if (atom->isInvalid())
            painter->setPen(COLOR_TEXT_UNKNOWN);
        else
            painter->setPen(COLOR_TEXT);

        painter->drawText(rect, Qt::AlignVCenter, text);
    }
}

void CircuitView::paintJack(QPainter *painter, JackAssignment *ja, const QColor textcolor, unsigned y)
{
    // CIRV_COLUMN 0: Name of the jack.
    painter->setPen(textcolor);
    painter->drawText(QRect(column_positions[0] + CIRV_TEXT_SIDE_PADDING, y, column_widths[0] - CIRV_TEXT_SIDE_PADDING, CIRV_JACK_HEIGHT), Qt::AlignVCenter, ja->jackName());
    painter->setPen(CIRV_COLOR_LINE);
    painter->drawLine(0, y, CIRV_WIDTH, y);

    // CIRV_COLUMN 1: A (first mult). Only for inputs
    if (ja->jackType() == JACKTYPE_INPUT)
    {
        JackAssignmentInput *jai = (JackAssignmentInput *)ja;
        for (int a=0; a<3; a++) {
            paintAtom(painter,
                      QRect(column_positions[a+1] + CIRV_TEXT_SIDE_PADDING,
                             y, column_widths[a+1] - CIRV_TEXT_SIDE_PADDING,
                             CIRV_JACK_HEIGHT),
                      jai->getAtom(a));
        }

        painter->setPen(CIRV_COLOR_LINE);
        paintOperator(painter, CIRV_OPERATOR_1_X, y, "✱");
        paintOperator(painter, CIRV_OPERATOR_2_X, y, "+");
        painter->setPen(CIRV_COLOR_LINE);
        painter->drawLine(column_positions[2], y, column_positions[2], y + CIRV_JACK_HEIGHT);
        painter->drawLine(column_positions[3], y, column_positions[3], y + CIRV_JACK_HEIGHT);
        painter->drawLine(CIRV_OPERATOR_1_X, y, CIRV_OPERATOR_1_X, y + CIRV_JACK_HEIGHT);
        painter->drawLine(CIRV_OPERATOR_2_X, y, CIRV_OPERATOR_2_X, y + CIRV_JACK_HEIGHT);

        // TODO: B and C
    }

    else {
        QString text;
        QRect rect(column_positions[1] + CIRV_TEXT_SIDE_PADDING, y, CIRV_COLUMN_123_WIDTH - CIRV_TEXT_SIDE_PADDING, CIRV_JACK_HEIGHT);
        if (ja->jackType() == JACKTYPE_OUTPUT)
        {
            JackAssignmentOutput *jao = (JackAssignmentOutput *)ja;
            paintAtom(painter, rect, jao->getAtom());
        }
        else  // UNKNOWN
        {
            JackAssignmentUnknown *jau = (JackAssignmentUnknown *)ja;
            text = jau->valueToString();
            painter->setPen(COLOR_TEXT_UNKNOWN);
            painter->drawText(rect, Qt::AlignVCenter, text);
        }
    }
}


void CircuitView::paintOperator(QPainter *painter, unsigned x, unsigned y, QString o)
{
    QRect r(x, y, CIRV_COLUMN_OPERATOR_WIDTH, CIRV_JACK_HEIGHT);
    painter->fillRect(r, CIRV_COLOR_OPERATOR_BACKGROUND);
    painter->setPen(CIRV_COLOR_OPERATOR);
    painter->save();
    painter->setPen(COLOR_TEXT);
    const QFont &font = painter->font();
    painter->setFont(QFont(font.family(), font.pointSize() * 1.2));
    painter->drawText(r, o, Qt::AlignVCenter | Qt::AlignCenter);
    painter->restore();
}

QRect CircuitView::headerRect() const
{
    return QRect(0, 0, CIRV_WIDTH, CIRV_HEADER_HEIGHT);
}

QRect CircuitView::commentRect() const
{
    return QRect(0, CIRV_HEADER_HEIGHT, CIRV_WIDTH, commentHeight());
}

QRect CircuitView::jackRect(int row) const
{
    return QRect(
                0,
                CIRV_HEADER_HEIGHT + commentHeight() + row * CIRV_JACK_HEIGHT,
                column_widths[0],
            CIRV_JACK_HEIGHT);
}

QRect CircuitView::atomRect(int row, int column) const
{
    JackAssignment *ja = circuit->jackAssignment(currentJack);
    int x, width;

    if (ja->jackType() == JACKTYPE_INPUT) {
        x = column_positions[column];
        width = column_widths[column];
    }
    else {
        x = column_positions[1];
        width = CIRV_COLUMN_123_WIDTH;
    }
    return QRect(x, jackRect(row).top(), width, CIRV_JACK_HEIGHT);
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
    if (x <= column_positions[1])
        return 0;
    else if (x <= CIRV_OPERATOR_1_X)
        return 1;
    else if (x <= CIRV_OPERATOR_2_X)
        return 2;
    else
        return 3;
}


int CircuitView::jackAt(unsigned y)
{
    if (y < CIRV_HEADER_HEIGHT)
        return -2;
    y -= CIRV_HEADER_HEIGHT;

    if (y < commentHeight())
        return -1;
    y -= commentHeight();

    int jack = y / CIRV_JACK_HEIGHT;
    if (jack >= circuit->numJackAssignments())
        return circuit->numJackAssignments() - 1;
    else
        return jack;
}

QPoint CircuitView::frameCursorPosition() const
{
    QPoint origin = pos().toPoint();
    int y = origin.y();
    if (currentJack == -1)
        y += CIRV_HEADER_HEIGHT;
    else if (currentJack >= 0)
        y += CIRV_HEADER_HEIGHT + commentHeight() + currentJack * CIRV_JACK_HEIGHT;

    int x = column_positions[currentColumn];
    return QPoint(x, y);
}
