#include "circuitinfoview.h"
#include "colorscheme.h"
#include "droidfirmware.h"
#include "tuning.h"
#include "globals.h"

#include <QPainter>
#include <QFontMetrics>


CircuitInfoView::CircuitInfoView(QString circuit, unsigned *width)
    : circuit(circuit)
    , selected(false)
    , circuitViewWidth(width)
{
}
QRectF CircuitInfoView::boundingRect() const
{
    return QRectF(0, 0, *circuitViewWidth, CICH_CIRCUIT_HEIGHT);
}
void CircuitInfoView::select(bool sel)
{
    selected = sel;
    update();
}
void CircuitInfoView::paintMultilineText(QPainter *painter, unsigned text_x, unsigned text_y, unsigned width, unsigned numLines, QString text)
{
    QFontMetrics fm = painter->fontMetrics();
    QStringList words = text.split(' ');

    QStringList lines;

    unsigned wordCounter = 0;
    QString lineText;
    while (wordCounter < words.count()) {
        QString word = words[wordCounter++];
        unsigned s = fm.horizontalAdvance(lineText + " " + word + "...");
        if (s > width) {
            lines.append(lineText);
            lineText = 0;
        }
        if (lineText != "")
            lineText += " ";
        lineText += word;
    }
    if (lineText != "")
        lines.append(lineText);

    if (lines.count() > numLines)
        lines[numLines-1] += "...";

    QRectF textRect(text_x, text_y, width, CICH_TITLE_HEIGHT);

    for (auto &line: lines.mid(0, numLines)) {
        painter->drawText(textRect, Qt::AlignTop | Qt::AlignLeft, line);
        textRect.translate(0, fm.lineSpacing());
    }
}
void CircuitInfoView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    unsigned text_x = CICH_ICON_WIDTH + 2*CICH_PADDING + STANDARD_SPACING;
    painter->fillRect(boundingRect(), COLOR(CICH_COLOR_CIRCUIT_BACKGROUND));

    // Circuit name
    painter->setPen(COLOR(CICH_COLOR_TITLE));
    painter->drawText(QRect(text_x, CICH_PADDING,
                      *circuitViewWidth - text_x, CICH_TITLE_HEIGHT),
                      Qt::AlignTop,
                      circuit.toUpper() + " - " +
                      the_firmware->circuitTitle(circuit));

    // Description
    QString description = the_firmware->circuitDescription(circuit);
    unsigned ramsize = the_firmware->circuitRamSize(circuit);
    QString ramString = QString::number(ramsize) + " " + TR("bytes");
    painter->setPen(COLOR(CICH_COLOR_DESCRIPTION));
    paintMultilineText(painter, text_x, CICH_PADDING + CICH_TITLE_HEIGHT, *circuitViewWidth - text_x - CICH_PADDING, 2, description);
    QRect ramsizeRect(*circuitViewWidth - 300, CICH_PADDING, 300 - CICH_PADDING, CICH_TITLE_HEIGHT);
    painter->setPen(COLOR(CICH_RAMSIZE_COLOR));
    painter->drawText(ramsizeRect, Qt::AlignRight, ramString);

    // Icon
    QImage image(QString(CIRCUIT_ICON_PATH +  circuit + CIRCUIT_ICON_SUFFIX));
    painter->drawImage(QRect(CICH_PADDING, CICH_PADDING,
                              CICH_ICON_WIDTH, CICH_ICON_WIDTH), image);

    // Cursor
    if (selected)
    {
        painter->setPen(COLOR(COLOR_CURSOR_NORMAL));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
    }
}
