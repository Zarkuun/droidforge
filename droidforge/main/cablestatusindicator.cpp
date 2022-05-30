
#include "cablestatusindicator.h"
#include "cablecolorizer.h"
#include "tuning.h"

#include <QDebug>

CableStatusIndicator::CableStatusIndicator(QWidget *parent)
    : QWidget{parent}
    , warningImage(":images/icons/warning.png") // TODO: Zentral ablegen?
    , animation(this, "animationPhase")
    , patching(false)
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
    setToolTip("");
    QPainter painter(this);
    if (patching)
        paintPatching(painter);
    else
        paintCableInfo(painter);
}

void CableStatusIndicator::paintPatching(QPainter &painter)
{
    painter.fillRect(rect(), CSD_BACKGROUND_COLOR);
    float left = CSD_SIDE_PADDING + animationPhase * CSD_ANIMATION_RANGE;
    float right = width() - CSD_SIDE_PADDING - (animationPhase * CSD_ANIMATION_RANGE);
    paintCable(painter, left, right);
    paintLabel(painter, width() / 2, tr("Patching..."));

    float imgHeight = height() - 2 * CSD_IMAGE_MARGIN;
    float imgWidth =  imgHeight * the_cable_colorizer->imageAspect();
    QRectF leftPlugRect(left, CSD_IMAGE_MARGIN, imgWidth, imgHeight);
    QRectF rightPlugRect(right - imgWidth, CSD_IMAGE_MARGIN, imgWidth, imgHeight);
    const QImage *plugImage = the_cable_colorizer->ghostPlug();
    QImage mirroredPlugImage = plugImage->mirrored(true, false);
    painter.drawImage(rightPlugRect, *plugImage);
    painter.drawImage(leftPlugRect, mirroredPlugImage);
}

void CableStatusIndicator::paintCableInfo(QPainter &painter)
{
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

    paintCable(painter, leftPlugRect.right(), rightPlugRect.left());
    const QImage *plugImage = the_cable_colorizer->imageForCable(cableName);
    QImage mirroredPlugImage = plugImage->mirrored(true, false);

    // Indicate number of times this cable is used as output
    if (numAsOutput) {
        painter.drawImage(leftPlugRect, mirroredPlugImage);
        if (numAsOutput > 1) {
            paintMarker(
                    painter,
                    leftPlugRect.right() + CSD_MARKER_DISTANCE,
                    CSD_BAD_MARKER_BORDER,
                    CSD_BAD_MARKER_BACKGROUND,
                    numAsOutput);
        }
    }

    // Indicate number of times this cable is used as input
    if (numAsInput) {
        painter.drawImage(rightPlugRect, *plugImage);
        if (numAsInput > 1) {
            paintMarker(
                    painter,
                    rightPlugRect.left() - CSD_MARKER_DISTANCE,
                    CSD_GOOD_MARKER_BORDER,
                    CSD_GOOD_MARKER_BACKGROUND,
                    numAsInput);
        }
    }

    // Finally print the name of the cable on top of all
    paintLabel(painter, (width() - problemMarkerWidth) / 2 + problemMarkerWidth, cableName);
}

void CableStatusIndicator::paintLabel(QPainter &painter, int xpos, QString text)
{
    const QFont &font = painter.font();
    QFont cableFont(font.family(), CSD_LABEL_FONT_SIZE);
    QFontMetrics fm(cableFont);
    int nameWidth = qMin(fm.horizontalAdvance(text), CSD_MAX_NAME_WIDTH) +
            CSD_NAME_PADDING * 2;
    QRectF nameRect(
                xpos - nameWidth / 2,
                CSD_IMAGE_MARGIN,
                nameWidth,
                height() - 2*CSD_IMAGE_MARGIN);
    painter.setBrush(CSD_LABEL_BACKGROUND);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(nameRect, 5, 5);
    painter.setPen(QColor(255, 255, 255));
    painter.setFont(cableFont);
    painter.drawText(nameRect, text, Qt::AlignVCenter | Qt::AlignCenter);

}

void CableStatusIndicator::paintMarker(QPainter &painter, int xpos, QColor border, QColor fill, int number)
{
    painter.save();
    float markerHeight = height() - 2 * CSD_IMAGE_MARGIN;
    QRectF markerRect(
                xpos - CSD_MARKER_WIDTH / 2,
                CSD_IMAGE_MARGIN,
                CSD_MARKER_WIDTH,
                markerHeight);

    painter.setPen(border);
    painter.setBrush(fill);
    painter.drawEllipse(markerRect);

    const QFont &font = painter.font();
    QFont markerFont(font.family(), CSD_MARKER_FONT_SIZE);
    markerFont.setLetterSpacing(QFont::PercentageSpacing, CSD_MARKER_LETTER_SPACING);

    painter.setFont(markerFont);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(
                markerRect,
                QString::number(number) + "X",
                Qt::AlignVCenter | Qt::AlignCenter);
    painter.restore();
}


void CableStatusIndicator::paintCable(QPainter &painter, int left, int right)
{
    // First paint the "cable"
    painter.save();
    float imgHeight = height() - 2 * CSD_IMAGE_MARGIN;
    cablePen.setWidth(imgHeight * CSD_CABLE_THICKNESS);
    painter.setPen(cablePen);
    painter.drawLine(left, height()/2, right, height()/2);
    painter.setPen(cableHilitePen);
    painter.drawLine(left, height()/2 - 1, right, height()/2 - 1);
    painter.restore();
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

void CableStatusIndicator::setPatchingState(bool p)
{
    if (p) {
        animation.setDuration(CSD_ANIMATION_DURATION);
        animation.setKeyValueAt(0, 0.0);
        animation.setKeyValueAt(0.8, 1.0);
        animation.setKeyValueAt(1.0, 0.0);
        animation.setEasingCurve(QEasingCurve::InQuad);
        animation.setLoopCount(-1); // forever
        animation.start();
    }
    else
        animation.stop();

    patching = p;
    update();
}

void CableStatusIndicator::setanimationPhase(float newanimationPhase)
{
    animationPhase = newanimationPhase;
    update();
    emit animationPhaseChanged(); // TODO: needed?
}


// Infos, die ich brauche

// Kabelname
// Anzahl Ouptuts
// Anzahl Inputs
// Status, ob ich grad verkable
