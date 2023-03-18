#include "circuitview.h"
#include "colorscheme.h"
#include "globals.h"
#include "iconbase.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "tuning.h"
#include "cablecolorizer.h"
#include "patchoperator.h"
#include "mainwindow.h"
#include "atomcable.h"

#include <QPainter>
#include <QFontMetrics>
#include <QFontDatabase>

#define NUM_COLUMNS 4

#define CIRV_TEXTMODE_EQUALS_WIDTH 20


CircuitView::CircuitView(MainWindow *mainWindow, Circuit *circuit, unsigned circuitNumber, const Selection * const*selection, float width, unsigned lineHeight)
    : mainWindow(mainWindow)
    , circuit(circuit)
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
    fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont = QFont(fixedFont.family(), 13);
}
float CircuitView::commentHeight() const
{
    if (circuit->hasComment())
        return circuit->numCommentLines() * lineHeight +
                (textMode() ? 0 : CIRV_COMMENT_PADDING * 2);
    else
        return 0;
}
float CircuitView::totalHeight() const
{
    if (isFolded())
        return headerHeight();
    else {
        unsigned num_jacks = circuit->numJackAssignments();
        return headerHeight()
             + commentHeight()
             + num_jacks * jackHeight();
    }
}
float CircuitView::headerHeight() const
{
    if (textMode())
        return lineHeight;
    else
        return CIRV_HEADER_HEIGHT;
}
float CircuitView::jackHeight() const
{
    if (textMode())
        return lineHeight;
    else
        return CIRV_JACK_HEIGHT;
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
    painter->setRenderHint(QPainter::Antialiasing); // Make lines, circles smooth
    if (textMode())
        painter->setFont(fixedFont);

    paintHeader(painter);
    if (!isFolded()) {
        if (circuit->hasComment())
            paintComment(painter);
        paintJacks(painter);
        if (!textMode())
            paintLines(painter);
    }
    paintSelection(painter);

    if (circuit->hasBookmark()) {
        QRectF cr = cellRect(circuit->bookmarkRow(), circuit->bookmarkColumn());
        QPen pen;
        pen.setColor(COLOR(COLOR_CURSOR_NORMAL));
        pen.setWidth(1);
        pen.setStyle(Qt::DotLine);
        painter->setPen(pen);
        painter->drawRect(cr);
    }
}
void CircuitView::paintHeader(QPainter *painter)
{
    if (textMode())
    {
        QString name;
        if (circuit->isDisabled()) {
            name = "# ";
            painter->setPen(COLOR(TEXTMODE_COMMENT));
        }
        else
            painter->setPen(COLOR(TEXTMODE_CIRCUIT));
        name += "[";
        name += circuit->getName().toLower() + "]";
        if (circuit->isFolded())
            name += " ...";
            painter->drawText(headerRect().translated(circuit->isDisabled() ? 0 : -3, 0), Qt::AlignVCenter, name);
    }
    else {
        // Icon and circuit name
        painter->fillRect(headerRect(), COLOR(CIRV_COLOR_CIRCUIT_NAME_BG));
        QRectF imageRect(headerRect().left(), headerRect().top(), headerHeight(), CIRV_HEADER_HEIGHT);
        if (circuit->isDisabled()) {
            QImage disIcon = iconImage.convertToFormat(QImage::Format_Grayscale8);
            painter->drawImage(imageRect.toRect(), disIcon);
        }
        else
            painter->drawImage(imageRect.toRect(), iconImage);

        // Circuit name
        painter->setPen(circuit->isDisabled() ? COLOR(CIRV_COLOR_DISABLED_TEXT) : COLOR(CIRV_COLOR_CIRCUIT_NAME));
        QRectF textRect =
                QRectF(headerRect().left() + headerHeight() + CIRV_ICON_MARGIN,
                       headerRect().top(),
                       headerRect().width() - headerHeight() - CIRV_ICON_MARGIN,
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
                         textRect.width() - CIRV_FOLDING_COMMENT_INDENT - 2 * headerHeight() - CIRV_ICON_MARGIN,
                         textRect.height());
                painter->drawText(r, Qt::AlignVCenter, oneliner);
            }
        }
    }
}
void CircuitView::paintComment(QPainter *painter)
{

    if (textMode()) {
        painter->setPen(COLOR(TEXTMODE_COMMENT));
        painter->drawText(commentRect().translated(0, 0.5),
                          Qt::AlignLeft | Qt::AlignTop,
                          circuit->getCommentWithHashes());
    }

    else {
        painter->setPen(COLOR(CIRV_COLOR_COMMENT));
        painter->fillRect(commentRect(), COLOR(CIRV_COLOR_COMMENT_BACKGROUND));
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
}
QRectF CircuitView::cellRect(int row, int column) const
{
    if (row == ROW_CIRCUIT)
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
void CircuitView::paintAtom(QPainter *painter, const QRectF &rect, QColor textcolor, Atom *atom, bool isInput, int row, int column)
{
    bool showDumpValue = false;
    double dumpValue;
    if (atom) {
        const StatusDump *dump = mainWindow->statusDump();
        if (dump && dump->hasAtom(atom)) {
            dumpValue = dump->valueOfAtom(atom);
            showDumpValue = true;
        }
    }
    painter->setPen(textcolor);

    if (textMode()) {
        if (atom) {
            if (textcolor == COLOR(TEXTMODE_COMMENT))
                ; // line is disabled, keep color
            else if (atom->isCable())
                painter->setPen(COLOR(TEXTMODE_CABLE));
            else if (atom->isRegister())
                painter->setPen(COLOR(TEXTMODE_REGISTER));
            else if (atom->isNumber())
                painter->setPen(COLOR(TEXTMODE_NUMBER));
            else
                painter->setPen(COLOR(TEXTMODE_INVALID));
            painter->drawText(rect, Qt::AlignVCenter, atom->toString());
            if (showDumpValue)
                paintDumpValue(painter, rect, dumpValue);
        }
        return;
    }

    int imageHeight = CIRV_CABLEPLUG_HEIGHT;
    int imageWidth = imageHeight * the_cable_colorizer->imageAspect();
    int imageTop = CIRV_CABLEPLUG_TOP_MARGING;

    QRectF textRect(
                rect.left() + CIRV_TEXT_SIDE_PADDING,
                rect.y(),
                rect.width() - 1 * CIRV_TEXT_SIDE_PADDING,
                rect.height());

    CursorPosition pos(circuitNumber, row, column);
    bool isPatchingFromHere =  theOperator()->isPatchingFrom(pos);
    QRectF r = textRect;
    QRectF imageRect(r.x(), r.y() + imageTop, imageWidth, imageHeight);
    QImage ghostPlug = *the_cable_colorizer->ghostPlug();
    if (!isInput)
        ghostPlug = ghostPlug.mirrored(true, false);

    if (atom) {
        if (isPatchingFromHere)
            painter->drawImage(imageRect, ghostPlug);
        else {
            QString text(tr(atom->toDisplay().toLatin1()));
            if (atom->isCable()) {
                text = text.mid(1);
                QImage image = *the_cable_colorizer->imageForCable(text);
                if (circuit->isDisabled())
                    image = *the_cable_colorizer->ghostPlug();

                if (isInput)
                    painter->drawImage(imageRect, image);
                else
                    painter->drawImage(imageRect, image.mirrored(true, false));
                r = r.translated(imageWidth + STANDARD_SPACING, 0);
                r.setWidth(r.width() - imageWidth - STANDARD_SPACING);
            }
            painter->drawText(r, Qt::AlignVCenter, text);
            if (showDumpValue)
                paintDumpValue(painter, rect, dumpValue);
        }
    }
    else if (isPatchingFromHere)
        painter->drawImage(imageRect, ghostPlug);
}
void CircuitView::paintDumpValue(QPainter *painter, const QRectF &rect, double value)
{
    painter->fillRect(rect, COLOR(CIRV_STATUS_DUMP_BACKGROUND));
    QRectF numberRect(
                rect.left(),
                rect.y(),
                rect.width() - CIRV_TEXT_SIDE_PADDING,
                rect.height());
    painter->setPen(COLOR(CIRV_STATUS_DUMP_TEXT));
    painter->drawText(
                numberRect,
                Qt::AlignVCenter | Qt::AlignRight,
                QString::number(value));
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
    if (c == 0) {
        if (textMode())
            return jackColumnWidth - CIRV_TEXTMODE_EQUALS_WIDTH;
        else
            return jackColumnWidth;
    }
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
    if (textMode() && c >= 0)
        pos += CIRV_TEXTMODE_EQUALS_WIDTH;
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
    QColor jackFgColor, jackBgColor, atomColor, operatorColor;
    atomColor = COLOR(CIRV_COLOR_TEXT);
    QColor bgColor = COLOR(row % 2 == 0 ? CIRV_COLOR_EVEN_ROW : CIRV_COLOR_ODD_ROW);

    if (ja->isDisabled()) {
        jackFgColor = COLOR(textMode() ? TEXTMODE_COMMENT : CIRV_COLOR_DISABLED_TEXT);
        atomColor = jackFgColor;
        bgColor = COLOR(CIRV_COLOR_DISABLED_JACK_BG);
        jackBgColor = COLOR(CIRV_COLOR_DISABLED_JACK_BG);
        operatorColor = jackFgColor;
    }
    else {
        operatorColor = COLOR(textMode() ? TEXTMODE_OPERATOR : CIRV_COLOR_OPERATOR);
        if (ja->jackType() == JACKTYPE_INPUT) {
            jackFgColor = COLOR(textMode() ? TEXTMODE_INPUT : CIRV_COLOR_INPUT_JACK);
            jackBgColor = COLOR(CIRV_COLOR_INPUT_JACK_BG);
        }
        else if (ja->jackType() == JACKTYPE_OUTPUT) {
            jackFgColor = COLOR(textMode() ? TEXTMODE_OUTPUT : CIRV_COLOR_OUTPUT_JACK);
            jackBgColor = COLOR(CIRV_COLOR_OUTPUT_JACK_BG);
        }
        else {
            atomColor = COLOR(textMode() ? TEXTMODE_INVALID : CIRV_COLOR_DISABLED_TEXT);
            jackFgColor = COLOR(textMode() ? TEXTMODE_INVALID : CIRV_COLOR_UNKNOWN_JACK);
            jackBgColor = COLOR(CIRV_COLOR_UNKNOWN_JACK_BG);
        }
    }

    if (textMode())
    {
        QString text = ja->jackName();
        if (ja->isDisabled())
            text = "#  " + text;
        painter->setPen(jackFgColor);
        painter->drawText(
                    QRectF(jr.left(),
                          jr.top(),
                          columnWidth(0),
                          jackHeight()), Qt::AlignVCenter, text);
        painter->setPen(operatorColor);
        painter->drawText(
                    QRectF(jr.left(),
                          jr.top(),
                          columnWidth(0) + 14,
                          jackHeight()), Qt::AlignVCenter | Qt::AlignRight, "=");
    }
    else {
        painter->fillRect(jackLineRect(row), bgColor);
        painter->fillRect(jr, jackBgColor);
        painter->setPen(jackFgColor);
        painter->drawText(
                    QRectF(jr.left() + CIRV_TEXT_SIDE_PADDING,
                          jr.top(),
                          columnWidth(0) - 2 * CIRV_TEXT_SIDE_PADDING,
                          jackHeight()), Qt::AlignVCenter, ja->jackName());
    }

    // CIRV_COLUMN 1: A (first mult). Only for inputs
    if (ja->jackType() == JACKTYPE_INPUT)
    {
        JackAssignmentInput *jai = (JackAssignmentInput *)ja;
        for (int a=0; a<3; a++) {
            QRectF ar = atomRect(row, a+1);
            paintAtom(painter, ar,  atomColor, jai->getAtom(a), true, row, a+1);
            if (*selection && (*selection)->atomSelected(circuitNumber, row, a+1))
                painter->fillRect(ar, COLOR(CIRV_COLOR_SELECTION));
        }

        QRectF ar = atomRect(row, 1);
        painter->setPen(operatorColor);
        if (jai->getAtom(1) || !textMode())
            paintOperator(painter, operatorPosition(0), ar.top(), "✱");
        if (jai->getAtom(2) || !textMode())
            paintOperator(painter, operatorPosition(1), ar.top(), "+");
        if (!textMode()) {
            painter->setPen(COLOR(CIRV_COLOR_LINE));
            painter->drawLine(columnPosition(2),   ar.top(), columnPosition(2),   ar.top() + jackHeight());
            painter->drawLine(columnPosition(3),   ar.top(), columnPosition(3),   ar.top() + jackHeight());
            painter->drawLine(operatorPosition(0), ar.top(), operatorPosition(0), ar.top() + jackHeight());
            painter->drawLine(operatorPosition(1), ar.top(), operatorPosition(1), ar.top() + jackHeight());
        }
    }

    else { // JACKTYPE_OUTPUT
        QRectF ar = atomRect(row, 1);
        if (ja->jackType() == JACKTYPE_OUTPUT)
        {
            JackAssignmentOutput *jao = (JackAssignmentOutput *)ja;
            paintAtom(painter, ar, atomColor, jao->getAtom(), false, row, 1);
        }
        else  // UNKNOWN
        {
            JackAssignmentUnknown *jau = (JackAssignmentUnknown *)ja;
            AtomInvalid atom(jau->valueToString());
            paintAtom(painter, ar, atomColor, &atom, false, row, 1);
        }
        if (*selection && (*selection)->atomSelected(circuitNumber, row, 1))
            painter->fillRect(ar, COLOR(CIRV_COLOR_SELECTION));
    }

    // horizontal line
    if (*selection && (*selection)->jackSelected(circuitNumber, row))
        painter->fillRect(jackLineRect(row), COLOR(CIRV_COLOR_SELECTION));

    if (!textMode()) {
        painter->setPen(COLOR(CIRV_COLOR_LINE));
        painter->drawLine(jr.left(), jr.top(), jr.left() + totalWidth, jr.top());
    }
}
void CircuitView::paintOperator(QPainter *painter, unsigned x, unsigned y, QString o)
{
    QRectF r(x, y, CIRV_COLUMN_OPERATOR_WIDTH, jackHeight());
    if (textMode()) {
        painter->drawText(r, o, Qt::AlignVCenter | Qt::AlignCenter);
    }
    else {
        painter->fillRect(r, COLOR(CIRV_COLOR_OPERATOR_BG));
        painter->save();
        const QFont &font = painter->font();
        painter->setFont(QFont(font.family(), font.pointSize() * 1.2));
        painter->drawText(r, o, Qt::AlignVCenter | Qt::AlignCenter);
        painter->restore();
    }
}
QRectF CircuitView::headerRect() const
{
    return QRectF(0, 0, totalWidth, headerHeight());
}
QRectF CircuitView::commentRect() const
{
    return QRectF(0, headerHeight(), totalWidth, commentHeight());
}
QRectF CircuitView::jackLineRect(int row) const
{
    return QRectF(0,
                headerHeight() + commentHeight() + row * jackHeight(),
                totalWidth,
                jackHeight());
}
QRectF CircuitView::jackRect(int row) const
{
    return QRectF(
                0,
                headerHeight() + commentHeight() + row * jackHeight(),
                columnWidth(0),
                jackHeight());
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
    return QRectF(x, jackRect(row).top(), width, jackHeight());
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
    if (y < headerHeight())
        return ROW_CIRCUIT;
    y -= headerHeight();

    if (y < commentHeight())
        return -1;
    y -= commentHeight();

    int jack = y / jackHeight();
    if (jack >= circuit->numJackAssignments())
        return circuit->numJackAssignments() - 1;
    else
        return jack;
}
int CircuitView::nextHeaderMarkerOffset()
{
    markerOffset -= headerHeight();
    return markerOffset;
}
PatchOperator *CircuitView::theOperator()
{
    return mainWindow->theOperator();
}
bool CircuitView::textMode() const
{
    return ACTION(ACTION_TEXT_MODE)->isChecked();
}
