#include "circuitview.h"
#include "colorscheme.h"
#include "globals.h"
#include "iconbase.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "tuning.h"
#include "cablecolorizer.h"

#include <QPainter>
#include <QFontMetrics>

#define NUM_COLUMNS 4


CircuitView::CircuitView(Circuit *circuit, unsigned circuitNumber, const Selection * const*selection, float width, unsigned lineHeight)
    : circuit(circuit)
    , circuitNumber(circuitNumber)
    , selection(selection)
    , totalWidth(width)
    , lineHeight(lineHeight)
    , icon(CIRCUIT_ICON_PATH + circuit->getName() + CIRCUIT_ICON_SUFFIX)
    , iconImage(CIRCUIT_ICON_PATH + circuit->getName() + CIRCUIT_ICON_SUFFIX)
    , markerOffset(0)
{
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
float CircuitView::totalHeight() const
{
    if (isFolded())
        return CIRV_HEADER_HEIGHT;
    else {
        unsigned num_jacks = circuit->numJackAssignments();
        return CIRV_HEADER_HEIGHT
             + commentHeight()
             + num_jacks * CIRV_JACK_HEIGHT;
    }
}
QRectF CircuitView::boundingRect() const
{
    return QRectF(0, 0, totalWidth, totalHeight());
}
unsigned CircuitView::minimumWidth()
{
    return
            CIRV_COLUMN_JACK_MINIMUM_WIDTH +
            CIRV_COLUMN_ATOM_MINIMUM_WIDTH * 3 +
            CIRV_COLUMN_OPERATOR_WIDTH * 2;
}
void CircuitView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    paintHeader(painter);
    if (!isFolded()) {
        if (circuit->hasComment())
            paintComment(painter);
        paintJacks(painter);
        paintLines(painter);
    }
    paintSelection(painter);
}
void CircuitView::paintHeader(QPainter *painter)
{
    // Icon and circuit name
    painter->fillRect(headerRect(), COLOR(CIRV_COLOR_CIRCUIT_NAME_BG));
    QRectF imageRect(headerRect().left(), headerRect().top(), CIRV_HEADER_HEIGHT, CIRV_HEADER_HEIGHT);
    if (circuit->isDisabled()) {
        QImage disIcon = iconImage.convertToFormat(QImage::Format_Grayscale8);        painter->fillRect(imageRect, QColor(80, 80, 80, 160)); // TODO: COLOR MAKRO
        painter->drawImage(imageRect.toRect(), disIcon);
    }
    else
        painter->drawImage(imageRect.toRect(), iconImage);

    // Circuit name
    painter->setPen(circuit->isDisabled() ? COLOR(CIRV_COLOR_DISABLED_TEXT) : COLOR(CIRV_COLOR_CIRCUIT_NAME));
    QRectF textRect =
                QRectF(headerRect().left() + CIRV_HEADER_HEIGHT + CIRV_ICON_MARGIN,
                      headerRect().top(),
                      headerRect().width() - CIRV_HEADER_HEIGHT - CIRV_ICON_MARGIN,
                      headerRect().height());
    painter->drawText(textRect,
                      Qt::AlignVCenter,
                circuit->getName().toUpper()); //  + (isFolded() ? " (FOLDED)" : ""));

    if (isFolded()) {
        if (circuit->hasComment()) {
            painter->setPen(COLOR(CIRV_COLOR_COMMENT));
            QString oneliner = circuit->getComment().replace("\n", "").simplified();
            QRectF r(textRect.left() + CIRV_FOLDING_COMMENT_INDENT,
                     textRect.top(),
                     textRect.width() - CIRV_FOLDING_COMMENT_INDENT - 2 * CIRV_HEADER_HEIGHT - CIRV_ICON_MARGIN,
                     textRect.height());
            painter->drawText(r, Qt::AlignVCenter, oneliner);
        }
    }
}
void CircuitView::paintComment(QPainter *painter)
{
    painter->fillRect(commentRect(), COLOR(CIRV_COLOR_COMMENT_BACKGROUND));
    painter->setPen(COLOR(CIRV_COLOR_COMMENT));
    painter->drawText(
                QRectF(commentRect().left() + CIRV_TEXT_SIDE_PADDING,
                      headerRect().bottom() + CIRV_COMMENT_PADDING,
                      commentRect().width() -  2 * CIRV_TEXT_SIDE_PADDING,
                      commentHeight()),
                      Qt::AlignLeft | Qt::AlignJustify | Qt::AlignTop,
                circuit->getComment());
    if (*selection && (*selection)->commentSelected(circuitNumber))
        painter->fillRect(commentRect(), COLOR(CIRV_COLOR_SELECTION));
}
QRectF CircuitView::cellRect(int row, int column) const
{
    if (row == ROW_CIRCUIT) // TODO: Endlich makros für ROW_CIRCUIT und -1 !!
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
    for (qsizetype row=0; row<circuit->numJackAssignments(); row++)
        paintJack(painter, circuit->jackAssignment(row), row);
}
void CircuitView::paintAtom(QPainter *painter, const QRectF &rect, QColor textcolor, Atom *atom, bool isInput)
{
    // TODO: Das hier aufraumen
    float aspect = the_cable_colorizer->imageAspect();
    int imageHeight = 15;
    int imageWidth = imageHeight * aspect;
    int imageTop = 4;

    QRectF textRect(
                rect.left() + CIRV_TEXT_SIDE_PADDING,
                rect.y(),
                rect.width() - 2 * CIRV_TEXT_SIDE_PADDING,
                rect.height());

    // painter->fillRect(rect, COLOR(CIRV_COLOR_ATOM_BACKGROUND));
    painter->setPen(textcolor);

    if (atom) {
        QString text(tr(atom->toDisplay().toLatin1()));
        QRectF r = textRect;
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
void CircuitView::paintLines(QPainter *painter)
{
    painter->setPen(COLOR(CIRV_COLOR_LINE));
    QRectF cr = boundingRect();
    painter->drawRect(cr); // borders
    painter->drawLine(
                columnPosition(1),
                commentRect().bottom(),
                columnPosition(1),
                cr.bottom()); // line after jack column
}
void CircuitView::paintSelection(QPainter *painter)
{
    QRectF cr = boundingRect();
    if (*selection && (*selection)->circuitSelected(circuitNumber))
        painter->fillRect(cr, COLOR(CIRV_COLOR_SELECTION));
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
    float pos = 0;
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
void CircuitView::paintJack(QPainter *painter, JackAssignment *ja, unsigned row)
{
    // CIRV_COLUMN 0: Name of the jack.
    QRectF jr = jackRect(row);
    QColor jackFgColor, jackBgColor, atomColor;
    atomColor = COLOR(CIRV_COLOR_TEXT);
    QColor bgColor = COLOR(row % 2 == 0 ? CIRV_COLOR_EVEN_ROW : CIRV_COLOR_ODD_ROW);

    if (ja->isDisabled()) {
        jackFgColor = COLOR(CIRV_COLOR_DISABLED_TEXT);
        atomColor = jackFgColor;
        bgColor = COLOR(CIRV_COLOR_DISABLED_JACK_BG);
        jackBgColor = COLOR(CIRV_COLOR_DISABLED_JACK_BG);
    }
    else if (ja->jackType() == JACKTYPE_INPUT) {
        jackFgColor = COLOR(CIRV_COLOR_INPUT_JACK);
        jackBgColor = COLOR(CIRV_COLOR_INPUT_JACK_BG);
    }
    else if (ja->jackType() == JACKTYPE_OUTPUT) {
        jackFgColor = COLOR(CIRV_COLOR_OUTPUT_JACK);
        jackBgColor = COLOR(CIRV_COLOR_OUTPUT_JACK_BG);
    }
    else {
        atomColor = COLOR(CIRV_COLOR_DISABLED_TEXT);
        jackFgColor = COLOR(CIRV_COLOR_UNKNOWN_JACK);
        jackBgColor = COLOR(CIRV_COLOR_UNKNOWN_JACK_BG);
    }

    painter->fillRect(jackLineRect(row), bgColor);
    painter->fillRect(jr, jackBgColor);
    painter->setPen(jackFgColor);
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
            paintAtom(painter, ar,  atomColor, jai->getAtom(a), true);
            if (*selection && (*selection)->atomSelected(circuitNumber, row, a+1))
                painter->fillRect(ar, COLOR(CIRV_COLOR_SELECTION));
        }

        QRectF ar = atomRect(row, 1);
        painter->setPen(jackFgColor);
        paintOperator(painter, operatorPosition(0), ar.top(), "✱");
        paintOperator(painter, operatorPosition(1), ar.top(), "+");
        painter->setPen(COLOR(CIRV_COLOR_LINE));
        painter->drawLine(columnPosition(2),   ar.top(), columnPosition(2),   ar.top() + CIRV_JACK_HEIGHT);
        painter->drawLine(columnPosition(3),   ar.top(), columnPosition(3),   ar.top() + CIRV_JACK_HEIGHT);
        painter->drawLine(operatorPosition(0), ar.top(), operatorPosition(0), ar.top() + CIRV_JACK_HEIGHT);
        painter->drawLine(operatorPosition(1), ar.top(), operatorPosition(1), ar.top() + CIRV_JACK_HEIGHT);
    }

    else { // JACKTYPE_OUTPUT
        QRectF ar = atomRect(row, 1);
        if (ja->jackType() == JACKTYPE_OUTPUT)
        {
            JackAssignmentOutput *jao = (JackAssignmentOutput *)ja;
            paintAtom(painter, ar, atomColor, jao->getAtom(), false);
        }
        else  // UNKNOWN
        {
            JackAssignmentUnknown *jau = (JackAssignmentUnknown *)ja;
            AtomInvalid atom(jau->valueToString());
            paintAtom(painter, ar, atomColor, &atom, false);
        }
        if (*selection && (*selection)->atomSelected(circuitNumber, row, 1))
            painter->fillRect(ar, COLOR(CIRV_COLOR_SELECTION));
    }

    if (*selection && (*selection)->jackSelected(circuitNumber, row))
        painter->fillRect(jackLineRect(row), COLOR(CIRV_COLOR_SELECTION));

    // horizontal line
    painter->setPen(COLOR(CIRV_COLOR_LINE));
    painter->drawLine(jr.left(), jr.top(), jr.left() + totalWidth, jr.top());
}
void CircuitView::paintOperator(QPainter *painter, unsigned x, unsigned y, QString o)
{
    QRectF r(x, y, CIRV_COLUMN_OPERATOR_WIDTH, CIRV_JACK_HEIGHT);
    painter->fillRect(r, COLOR(CIRV_COLOR_OPERATOR_BG));
    painter->save();
    const QFont &font = painter->font();
    painter->setFont(QFont(font.family(), font.pointSize() * 1.2));
    painter->setPen(COLOR(CIRV_COLOR_OPERATOR));
    painter->drawText(r, o, Qt::AlignVCenter | Qt::AlignCenter);
    painter->restore();
}
QRectF CircuitView::headerRect() const
{
    return QRectF(0, 0, totalWidth, CIRV_HEADER_HEIGHT);
}
QRectF CircuitView::commentRect() const
{
    return QRectF(0, CIRV_HEADER_HEIGHT, totalWidth, commentHeight());
}
QRectF CircuitView::jackLineRect(int row) const
{
    return QRectF(0,
                CIRV_HEADER_HEIGHT + commentHeight() + row * CIRV_JACK_HEIGHT,
                totalWidth,
                CIRV_JACK_HEIGHT);
}
QRectF CircuitView::jackRect(int row) const
{
    return QRectF(
                0,
                CIRV_HEADER_HEIGHT + commentHeight() + row * CIRV_JACK_HEIGHT,
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
    if (y < CIRV_HEADER_HEIGHT)
        return ROW_CIRCUIT;
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
int CircuitView::nextHeaderMarkerOffset()
{
    markerOffset -= CIRV_HEADER_HEIGHT;
    return markerOffset;
}
