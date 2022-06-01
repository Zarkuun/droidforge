#include "circuitview.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "tuning.h"
#include "cablecolorizer.h"

#include <QPainter>
#include <QFontMetrics>

#define NUM_COLUMNS 4


CircuitView::CircuitView(Circuit *circuit, unsigned circuitNumber, const Selection * const*selection, float width, unsigned lineHeight, unsigned bottomPadding)
    : circuit(circuit)
    , circuitNumber(circuitNumber)
    , selection(selection)
    , totalWidth(width)
    , lineHeight(lineHeight)
    , bottomPadding(bottomPadding)
    , icon(CIRCUIT_ICON_PATH + circuit->getName() + CIRCUIT_ICON_SUFFIX)
{
    effect.setBlurRadius(15);
    effect.setColor(QColor(0,0,0));
    effect.setOffset(0, 0);
    effect.setEnabled(false);
    setGraphicsEffect(&effect);

    // Compute width of columns

    float sparePerColumn = (totalWidth - minimumWidth()) / NUM_COLUMNS;
    if (sparePerColumn < 0)
        sparePerColumn = 0;
    jackColumnWidth = CIRV_COLUMN_JACK_MINIMUM_WIDTH + sparePerColumn;
    atomColumnWidth = CIRV_COLUMN_ATOM_MINIMUM_WIDTH + sparePerColumn;
}

float CircuitView::commentHeight() const
{
    if (circuit->hasComment())
        return circuit->numCommentLines() * lineHeight
                + CIRV_COMMENT_PADDING * 2;
    else
        return 0;
}


float CircuitView::contentHeight() const
{
    unsigned num_jacks = circuit->numJackAssignments();
    return CIRV_HEADER_HEIGHT
         + commentHeight()
         + num_jacks * CIRV_JACK_HEIGHT;
}


QRectF CircuitView::boundingRect() const
{
    // unsigned height = contentHeight() + PATV_CIRCUIT_VERTICAL_MARGIN;
    return QRectF(
                0,
                0,
                totalWidth,
                contentHeight() + CIRV_TOP_PADDING + bottomPadding);
}

unsigned CircuitView::minimumWidth()
{
    return
            CIRV_COLUMN_JACK_MINIMUM_WIDTH +
            CIRV_COLUMN_ATOM_MINIMUM_WIDTH * 3 +
            CIRV_COLUMN_OPERATOR_WIDTH * 2 +
            CIRV_SIDE_PADDING * 2;
}

void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF cr = contentRect();
    painter->fillRect(cr, CIRV_COLOR_BACKGROUND);

    // Icon and circuit name
    painter->fillRect(headerRect(), CIRV_COLOR_CIRCUIT_NAME_BACKGROUND);
    QRectF imageRect(headerRect().left(), headerRect().top(), CIRV_HEADER_HEIGHT, CIRV_HEADER_HEIGHT);
    painter->drawPixmap(imageRect.toRect(), icon);
    if (circuit->isDisabled())
        painter->fillRect(imageRect, QColor(80, 80, 80, 160));
    painter->setPen(circuit->isDisabled() ? COLOR_TEXT_DISABLED : CIRV_COLOR_CIRCUIT_NAME);
    painter->drawText(
                QRectF(headerRect().left() + CIRV_HEADER_HEIGHT + CIRV_ICON_MARGIN,
                      headerRect().top(),
                      headerRect().width() - CIRV_HEADER_HEIGHT - CIRV_ICON_MARGIN,
                      headerRect().height()),
                      Qt::AlignVCenter, circuit->getName().toUpper());

    // Comment
    painter->fillRect(commentRect(), CIRV_COLOR_COMMENT_BACKGROUND);
    painter->setPen(CIRV_COLOR_COMMENT);
    painter->drawText(
                QRectF(commentRect().left() + CIRV_TEXT_SIDE_PADDING,
                      headerRect().bottom() + CIRV_COMMENT_PADDING,
                      commentRect().width() -  2 * CIRV_TEXT_SIDE_PADDING,
                      commentHeight()),
                      Qt::AlignLeft | Qt::AlignJustify | Qt::AlignTop, circuit->getComment());
    if (*selection && (*selection)->commentSelected(circuitNumber))
        painter->fillRect(commentRect(), CIRV_COLOR_SELECTION);

    // Jacks
    paintJacks(painter);

    // Lines
    painter->setPen(CIRV_COLOR_LINE);
    painter->drawRect(cr); // borders
    painter->drawLine(
                columnPosition(1),
                commentRect().bottom(),
                columnPosition(1),
                cr.bottom()); // line after jack column

    if (*selection && (*selection)->circuitSelected(circuitNumber))
        painter->fillRect(cr, CIRV_COLOR_SELECTION);
}

QRectF CircuitView::cellRect(int row, int column) const
{
    if (row == -2) // TODO: Endlich makros für -2 und -1 !!
        return headerRect();
    else if (row == -1)
        return commentRect();
    else if (column == 0)
        return jackRect(row);
    else
        return atomRect(row, column);
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

void CircuitView::paintJacks(QPainter *painter)
{
    painter->save();
    for (qsizetype row=0; row<circuit->numJackAssignments(); row++) {
        JackAssignment *ja = circuit->jackAssignment(row);
        QColor textcolor;
        if (ja->isDisabled())
            textcolor = COLOR_TEXT_DISABLED;
        else {
            switch (ja->jackType()) {
            case JACKTYPE_INPUT: textcolor = COLOR_JACK_INPUT; break;
            case JACKTYPE_OUTPUT: textcolor = COLOR_JACK_OUTPUT; break;
            default: textcolor = COLOR_JACK_UNKNOWN; break;
            }
        }
        paintJack(painter, ja, textcolor, row);
    }
    painter->restore();
}


void CircuitView::paintAtom(QPainter *painter, const QRectF &rect, QColor textcolor, Atom *atom, bool isInput)
{
    // TODO: Das hier aufraumen
    float aspect = the_cable_colorizer->imageAspect();
    int imageHeight = 15;
    int imageWidth = imageHeight * aspect;
    int imageTop = 4;

    painter->setPen(textcolor);

    if (atom) {
        QString text(tr(atom->toDisplay().toLatin1()));
        if (atom->isInvalid()) {
            // const int offset = (CIRV_JACK_HEIGHT - CIRV_ICON_WIDTH) / 2;
            QRectF textRect(
                        rect.x(),
                        rect.y(),
                        rect.width() - CIRV_JACK_HEIGHT - CIRV_TEXT_SIDE_PADDING,
                        rect.height());
            if (textcolor != COLOR_TEXT_DISABLED)
                painter->setPen(COLOR_TEXT_UNKNOWN);
            painter->drawText(textRect, Qt::AlignVCenter, text);
        }
        else {
            QRectF r = rect;
            if (atom->isCable()) {
                text = text.mid(1);
                QRectF imageRect(r.x(), r.y() + imageTop, imageWidth, imageHeight);
                const QImage *image = the_cable_colorizer->imageForCable(text);
                if (isInput)
                    painter->drawImage(imageRect, *image);
                else
                    painter->drawImage(imageRect, (*image).mirrored(true, false));
                r = r.translated(imageWidth + STANDARD_SPACING, 0);
            }
            painter->drawText(r, Qt::AlignVCenter, text);
        }
    }
}

QRectF CircuitView::contentRect() const
{
    return QRectF(CIRV_SIDE_PADDING, CIRV_TOP_PADDING, contentWidth(), contentHeight());
}

float CircuitView::contentWidth() const
{
    return totalWidth - 2 * CIRV_SIDE_PADDING;
}

float CircuitView::columnWidth(int c) const
{
    if (c == 0)
        return jackColumnWidth;
    else
        return atomColumnWidth;
}

float CircuitView::column123Width() const
{
    return 3 * atomColumnWidth + 2 * CIRV_COLUMN_OPERATOR_WIDTH;
}

float CircuitView::columnPosition(int c) const
{
    float pos = CIRV_SIDE_PADDING;
    for (int i=0; i<c; i++)
        pos += columnWidth(i);
    if (c >= 2)
        pos += CIRV_COLUMN_OPERATOR_WIDTH;
    if (c >= 3)
        pos += CIRV_COLUMN_OPERATOR_WIDTH;
    return pos;
}

float CircuitView::operatorPosition(int o) const
{
    return columnPosition(1 + o) + columnWidth(1 + o);
}

void CircuitView::paintJack(QPainter *painter, JackAssignment *ja, const QColor textcolor, unsigned row)
{
    // CIRV_COLUMN 0: Name of the jack.
    QRectF jr = jackRect(row);
    painter->setPen(textcolor);
    painter->drawText(
                QRectF(jr.left() + CIRV_TEXT_SIDE_PADDING,
                      jr.top(),
                      columnWidth(0) - 2 * CIRV_TEXT_SIDE_PADDING,
                      CIRV_JACK_HEIGHT), Qt::AlignVCenter, ja->jackName());

    // CIRV_COLUMN 1: A (first mult). Only for inputs
    if (ja->jackType() == JACKTYPE_INPUT)
    {
        JackAssignmentInput *jai = (JackAssignmentInput *)ja;
        for (int a=0; a<3; a++) {
            QRectF ar = atomRect(row, a+1);
            paintAtom(painter,
                      QRectF(ar.left() + CIRV_TEXT_SIDE_PADDING,
                          ar.top(),
                          ar.width() - 2 * CIRV_TEXT_SIDE_PADDING,
                          ar.height()),
                      textcolor,
                      jai->getAtom(a),
                      true);
            if (*selection && (*selection)->atomSelected(circuitNumber, row, a+1))
                painter->fillRect(ar, CIRV_COLOR_SELECTION);
        }

        QRectF ar = atomRect(row, 1);
        painter->setPen(textcolor);
        paintOperator(painter, operatorPosition(0), ar.top(), "✱");
        paintOperator(painter, operatorPosition(1), ar.top(), "+");
        painter->setPen(CIRV_COLOR_LINE);
        painter->drawLine(columnPosition(2),   ar.top(), columnPosition(2),   ar.top() + CIRV_JACK_HEIGHT);
        painter->drawLine(columnPosition(3),   ar.top(), columnPosition(3),   ar.top() + CIRV_JACK_HEIGHT);
        painter->drawLine(operatorPosition(0), ar.top(), operatorPosition(0), ar.top() + CIRV_JACK_HEIGHT);
        painter->drawLine(operatorPosition(1), ar.top(), operatorPosition(1), ar.top() + CIRV_JACK_HEIGHT);
    }

    else { // JACKTYPE_OUTPUT
        QRectF ar = atomRect(row, 1);
        QString text;
        QRectF rect(columnPosition(1) + CIRV_TEXT_SIDE_PADDING,
                   ar.top(),
                   column123Width() - CIRV_TEXT_SIDE_PADDING,
                   CIRV_JACK_HEIGHT);
        if (ja->jackType() == JACKTYPE_OUTPUT)
        {
            JackAssignmentOutput *jao = (JackAssignmentOutput *)ja;
            paintAtom(painter, rect, textcolor, jao->getAtom(), false);
        }
        else  // UNKNOWN
        {
            JackAssignmentUnknown *jau = (JackAssignmentUnknown *)ja;
            text = jau->valueToString();
            painter->setPen(COLOR_TEXT_UNKNOWN);
            painter->drawText(rect, Qt::AlignVCenter, text);
        }
        if (*selection && (*selection)->atomSelected(circuitNumber, row, 1))
            painter->fillRect(ar, CIRV_COLOR_SELECTION);
    }

    if (*selection && (*selection)->jackSelected(circuitNumber, row))
        painter->fillRect(jackLineRect(row), CIRV_COLOR_SELECTION);

    // horizontal line
    painter->setPen(CIRV_COLOR_LINE);
    painter->drawLine(jr.left(), jr.top(), jr.left() + contentWidth(), jr.top());
}


void CircuitView::paintOperator(QPainter *painter, unsigned x, unsigned y, QString o)
{
    QRectF r(x, y, CIRV_COLUMN_OPERATOR_WIDTH, CIRV_JACK_HEIGHT);
    painter->fillRect(r, CIRV_COLOR_OPERATOR_BACKGROUND);
    painter->save();
    const QFont &font = painter->font();
    painter->setFont(QFont(font.family(), font.pointSize() * 1.2));
    painter->drawText(r, o, Qt::AlignVCenter | Qt::AlignCenter);
    painter->restore();
}

QRectF CircuitView::headerRect() const
{
    return QRectF(CIRV_SIDE_PADDING, CIRV_TOP_PADDING,
                 totalWidth - 2 * CIRV_SIDE_PADDING,
                 CIRV_HEADER_HEIGHT);
}

QRectF CircuitView::commentRect() const
{
    return QRectF(CIRV_SIDE_PADDING,
                 CIRV_TOP_PADDING + CIRV_HEADER_HEIGHT,
                 totalWidth - 2 * CIRV_SIDE_PADDING,
                 commentHeight());
}

QRectF CircuitView::jackLineRect(int row) const
{
    return QRectF(
                CIRV_SIDE_PADDING,
                CIRV_TOP_PADDING + CIRV_HEADER_HEIGHT + commentHeight() + row * CIRV_JACK_HEIGHT,
                totalWidth - 2 * CIRV_SIDE_PADDING,
                CIRV_JACK_HEIGHT);
}

QRectF CircuitView::jackRect(int row) const
{
    return QRectF(
                CIRV_SIDE_PADDING,
                CIRV_TOP_PADDING + CIRV_HEADER_HEIGHT + commentHeight() + row * CIRV_JACK_HEIGHT,
                columnWidth(0),
                CIRV_JACK_HEIGHT);
}

QRectF CircuitView::atomRect(int row, int column) const
{
    JackAssignment *ja = circuit->jackAssignment(row);
    int x, width;

    if (ja->jackType() == JACKTYPE_INPUT) {
        x = columnPosition(column);
        width = columnWidth(column);
    }
    else {
        x = columnPosition(1);
        width = column123Width();
    }
    return QRectF(x, jackRect(row).top(), width, CIRV_JACK_HEIGHT);
}

int CircuitView::columnAt(unsigned x)
{
    if (x <= columnPosition(1))
        return 0;
    else if (x <= operatorPosition(0))
        return 1;
    else if (x <= operatorPosition(1))
        return 2;
    else
        return 3;
}

int CircuitView::jackAt(unsigned y)
{
    if (y < CIRV_HEADER_HEIGHT + CIRV_TOP_PADDING)
        return -2;
    y -= CIRV_HEADER_HEIGHT + CIRV_TOP_PADDING;

    if (y < commentHeight())
        return -1;
    y -= commentHeight();

    int jack = y / CIRV_JACK_HEIGHT;
    if (jack >= circuit->numJackAssignments())
        return circuit->numJackAssignments() - 1;
    else
        return jack;
}
