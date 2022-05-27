#include "cablestatusindicator.h"
#include "cablecolorizer.h"
#include "tuning.h"

#include <QDebug>

CableStatusIndicator::CableStatusIndicator(QWidget *parent)
    : QWidget{parent}
    , warningImage(":images/icons/warning.png") // TODO: Zentral ablegen?
{
    resize(400, 100);
    setMinimumWidth(CSD_WIDTH);
    setMaximumWidth(CSD_WIDTH);

    cablePen.setColor(CSD_CABLE_COLOR);
    cableHilitePen.setColor(CSD_CABLE_HILITE_COLOR);
    cableHilitePen.setWidth(1);
}

void CableStatusIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    setToolTip("");
    if (cableName == "") {
        return;
    }

    painter.fillRect(rect(), CSD_BACKGROUND_COLOR);

    float imgHeight = height() - 2 * CSD_IMAGE_MARGIN;
    float imgWidth =  imgHeight * the_cable_colorizer->imageAspect();
    bool haveProblem = numAsOutput != 1 || numAsInput == 0;
    unsigned problemMarkerWidth = haveProblem ? (imgHeight + STANDARD_SPACING / 2) : 0;
    QRectF leftPlugRect(CSD_SIDE_PADDING + problemMarkerWidth, CSD_IMAGE_MARGIN, imgWidth, imgHeight);
    QRectF rightPlugRect(width() - CSD_SIDE_PADDING - imgWidth, CSD_IMAGE_MARGIN, imgWidth, imgHeight);

    if (haveProblem) {
        QRectF warnRect(CSD_SIDE_PADDING,
                        leftPlugRect.top(),
                        imgHeight,
                        imgHeight);
        painter.drawImage(warnRect, warningImage);

        QString problem;
        if (numAsOutput == 0)
            problem = tr("The cable %1 is not connected to any output.");
        else if (numAsOutput > 1)
            problem = tr("The cable %1 is connected to more than one output.");
        else if (numAsInput == 0)
            problem = tr("The cable %1 is not connected to any input.");

        setToolTip(problem.arg(cableName));
    }

    // First paint the "cable"
    cablePen.setWidth(imgHeight * CSD_CABLE_THICKNESS);
    painter.setPen(cablePen);
    painter.drawLine(leftPlugRect.right(), height()/2, rightPlugRect.left(), height()/2);

    painter.setPen(cableHilitePen);
    painter.drawLine(leftPlugRect.right(), height()/2 - 1, rightPlugRect.left(), height()/2 - 1);

    const QImage *plugImage = the_cable_colorizer->imageForCable(cableName);
    QImage mirroredPlugImage = plugImage->mirrored(true, false);

    // Markers with number of usages
    unsigned markerHeight = imgHeight;
    unsigned markerWidth = 20; // markerHeight * CSD_MARKER_WIDTH;
    const QFont &font = painter.font();
    QFont markerFont(font.family(), CSD_MARKER_FONT_SIZE);
    markerFont.setLetterSpacing(QFont::PercentageSpacing, CSD_MARKER_LETTER_SPACING);

    // Indicate number of times this cable is used as output
    if (numAsOutput) {
        painter.drawImage(leftPlugRect, mirroredPlugImage);
        if (numAsOutput > 1) {
            QRectF markerRect(
                          leftPlugRect.right() + CSD_MARKER_DISTANCE,
                          leftPlugRect.top(),
                          markerWidth,
                          markerHeight);
            painter.setBrush(CSD_BAD_MARKER_BACKGROUND);
            painter.setPen(CSD_BAD_MARKER_BORDER);
            painter.drawEllipse(markerRect);
            painter.setFont(markerFont);
            painter.setPen(QColor(255, 255, 255));
            painter.drawText(
                        markerRect,
                        QString::number(numAsOutput) + "X",
                        Qt::AlignVCenter | Qt::AlignCenter);
        }
    }

    // Indicate number of times this cable is used as input
    if (numAsInput) {
        painter.drawImage(rightPlugRect, *plugImage);
        if (numAsInput > 1) {
            QRectF markerRect(
                          rightPlugRect.left() - CSD_MARKER_DISTANCE - markerHeight,
                          leftPlugRect.top(),
                          markerWidth,
                          markerHeight);

            painter.setBrush(CSD_GOOD_MARKER_BACKGROUND);
            painter.setPen(CSD_GOOD_MARKER_BORDER);
            painter.drawEllipse(markerRect);
            painter.setFont(markerFont);
            painter.setPen(QColor(255, 255, 255));
            painter.drawText(
                        markerRect,
                        QString::number(numAsInput) + "X",
                        Qt::AlignVCenter | Qt::AlignCenter);
        }
    }

    // Finally print the name of the cable on top of all
    QFont cableFont(font.family(), CSD_LABEL_FONT_SIZE);
    cableFont.setLetterSpacing(QFont::PercentageSpacing, CSD_MARKER_LETTER_SPACING);
    QFontMetrics fm(cableFont);
    int nameWidth = qMin(fm.horizontalAdvance(cableName), CSD_MAX_NAME_WIDTH) +
            CSD_NAME_PADDING * 2;
    QRectF nameRect(
                (width() - problemMarkerWidth) / 2 - nameWidth / 2 + problemMarkerWidth,
                leftPlugRect.top(),
                nameWidth,
                leftPlugRect.height());
    painter.setBrush(CSD_LABEL_BACKGROUND);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(nameRect, 5, 5);
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(cableFont);
    painter.drawText(nameRect, cableName, Qt::AlignVCenter | Qt::AlignCenter);

}

void CableStatusIndicator::set(QString name, int numAsIn, int numAsOut)
{
    numAsInput = numAsIn;
    numAsOutput = numAsOut;
    cableName = name;
    update();
}

void CableStatusIndicator::clear()
{
    cableName = "";
    update();
}


// Infos, die ich brauche

// Kabelname
// Anzahl Ouptuts
// Anzahl Inputs
// Status, ob ich grad verkable
