#include "cablestatusindicator.h"
#include "atomcable.h"
#include "cablecolorizer.h"
#include "colorscheme.h"
#include "tuning.h"
#include "updatehub.h"
#include "editoractions.h"

#include <QMouseEvent>

CableStatusIndicator::CableStatusIndicator(PatchEditEngine *patch, QWidget *parent)
    : QWidget{parent}
    , PatchView(patch)
    , warningImage(":images/icons/warning.png") // TODO: Zentral ablegen?
    , animation(this, "animationPhase")
{
    resize(400, 100); // TODO: Was ist hiermit?
    setMinimumWidth(CSI_WIDTH);
    setMaximumWidth(CSI_WIDTH);

    cablePen.setColor(COLOR(CSI_CABLE_COLOR));
    cableHilitePen.setColor(COLOR(CSI_CABLE_HILITE_COLOR));
    cableHilitePen.setWidth(1);

    // TODO: mouse click soll signal machen bzw. direkt
    // eine Aktion triggern (follow cable). Und eine Hand will ich
    // sehen.
    SET_ACTION_TRIGGER(ACTION_ADD_JACK, &CableStatusIndicator::clicked);

    // Events that we are interested in
    connect(the_hub, &UpdateHub::patchModified, this, &CableStatusIndicator::updateStatus);
    connect(the_hub, &UpdateHub::cursorMoved, this, &CableStatusIndicator::updateStatus);
    connect(the_hub, &UpdateHub::patchingChanged, this, &CableStatusIndicator::changePatching);
}

void CableStatusIndicator::paintEvent(QPaintEvent *)
{
    setToolTip("");
    QPainter painter(this);
    if (patch->isPatching())
        paintPatching(painter);
    else
        paintCableInfo(painter);
}

void CableStatusIndicator::mousePressEvent(QMouseEvent *event)
{
    // TODO: Vom Status abhängig machen
    if (event->button() == Qt::LeftButton)
        emit clicked();
}

void CableStatusIndicator::paintPatching(QPainter &painter)
{
    painter.fillRect(rect(), COLOR(COLOR_STATUSBAR_BACKGROUND));
    float left = CSI_SIDE_PADDING + animationPhase * CSI_ANIMATION_RANGE;
    float right = width() - CSI_SIDE_PADDING - (animationPhase * CSI_ANIMATION_RANGE);
    paintCable(painter, left, right);
    paintLabel(painter, width() / 2, tr("Patching..."));

    float imgHeight = height() - 2 * CSI_IMAGE_MARGIN;
    float imgWidth =  imgHeight * the_cable_colorizer->imageAspect();
    QRectF leftPlugRect(left, CSI_IMAGE_MARGIN, imgWidth, imgHeight);
    QRectF rightPlugRect(right - imgWidth, CSI_IMAGE_MARGIN, imgWidth, imgHeight);
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
    painter.fillRect(rect(), COLOR(COLOR_STATUSBAR_BACKGROUND));

    float imgHeight = height() - 2 * CSI_IMAGE_MARGIN;
    float imgWidth =  imgHeight * the_cable_colorizer->imageAspect();
    bool haveProblem = numAsOutput != 1 || numAsInput == 0;
    unsigned problemMarkerWidth = haveProblem ? (imgHeight + STANDARD_SPACING / 2) : 0;
    QRectF leftPlugRect(CSI_SIDE_PADDING + problemMarkerWidth, CSI_IMAGE_MARGIN, imgWidth, imgHeight);
    QRectF rightPlugRect(width() - CSI_SIDE_PADDING - imgWidth, CSI_IMAGE_MARGIN, imgWidth, imgHeight);

    if (haveProblem) {
        QRectF warnRect(CSI_SIDE_PADDING,
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
                    leftPlugRect.right() + CSI_MARKER_DISTANCE,
                    COLOR(CSI_BAD_MARKER_BORDER),
                    COLOR(CSI_BAD_MARKER_BACKGROUND),
                    numAsOutput);
        }
    }

    // Indicate number of times this cable is used as input
    if (numAsInput) {
        painter.drawImage(rightPlugRect, *plugImage);
        if (numAsInput > 1) {
            paintMarker(
                    painter,
                    rightPlugRect.left() - CSI_MARKER_DISTANCE,
                    COLOR(CSI_GOOD_MARKER_BORDER),
                    COLOR(CSI_GOOD_MARKER_BACKGROUND),
                    numAsInput);
        }
    }

    // Finally print the name of the cable on top of all
    paintLabel(painter, (width() - problemMarkerWidth) / 2 + problemMarkerWidth, cableName);
}

void CableStatusIndicator::paintLabel(QPainter &painter, int xpos, QString text)
{
    const QFont &font = painter.font();
    QFont cableFont(font.family(), CSI_LABEL_FONT_SIZE);
    QFontMetrics fm(cableFont);
    int nameWidth = qMin(fm.horizontalAdvance(text), CSI_MAX_NAME_WIDTH) +
            CSI_NAME_PADDING * 2;
    QRectF nameRect(
                xpos - nameWidth / 2,
                CSI_IMAGE_MARGIN,
                nameWidth,
                height() - 2*CSI_IMAGE_MARGIN);
    painter.setBrush(COLOR(CSI_LABEL_BACKGROUND));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(nameRect, 5, 5);
    painter.setPen(COLOR(CSI_LABEL_COLOR));
    painter.setFont(cableFont);
    painter.drawText(nameRect, text, Qt::AlignVCenter | Qt::AlignCenter);

}

void CableStatusIndicator::paintMarker(QPainter &painter, int xpos, QColor border, QColor fill, int number)
{
    painter.save();
    float markerHeight = height() - 2 * CSI_IMAGE_MARGIN;
    QRectF markerRect(
                xpos - CSI_MARKER_WIDTH / 2,
                CSI_IMAGE_MARGIN,
                CSI_MARKER_WIDTH,
                markerHeight);

    painter.setPen(border);
    painter.setBrush(fill);
    painter.drawEllipse(markerRect);

    const QFont &font = painter.font();
    QFont markerFont(font.family(), CSI_MARKER_FONT_SIZE);
    markerFont.setLetterSpacing(QFont::PercentageSpacing, CSI_MARKER_LETTER_SPACING);

    painter.setFont(markerFont);
    painter.setPen(COLOR(CSI_MARKER_TEXT_COLOR));
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
    float imgHeight = height() - 2 * CSI_IMAGE_MARGIN;
    cablePen.setWidth(imgHeight * CSI_CABLE_THICKNESS);
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

void CableStatusIndicator::updatePatchingAnimation()
{
    if (patch->isPatching()) {
        animation.setDuration(CSI_ANIMATION_DURATION);
        animation.setKeyValueAt(0, 0.0);
        animation.setKeyValueAt(0.8, 1.0);
        animation.setKeyValueAt(1.0, 0.0);
        animation.setEasingCurve(QEasingCurve::InQuad);
        animation.setLoopCount(-1); // forever
        animation.start();
    }
    else
        animation.stop();
    update();
}

void CableStatusIndicator::setanimationPhase(float newanimationPhase)
{
    animationPhase = newanimationPhase;
    update();
    emit animationPhaseChanged(); // TODO: needed?
}

void CableStatusIndicator::updateStatus()
{
    const Atom *atom = patch->currentAtom();
    if (atom && atom->isCable()) {
        AtomCable *ac = (AtomCable *)atom;
        QString name = ac->getCable();
        int numAsOutput = 0;
        int numAsInput = 0;
        patch->findCableConnections(name, numAsInput, numAsOutput);
        set(name, numAsInput, numAsOutput);
        setCursor(Qt::PointingHandCursor);
    }
    else {
        clear();
        unsetCursor();
    }
}

void CableStatusIndicator::changePatching()
{
    updateStatus();
    updatePatchingAnimation();
}

// Status, ob ich grad verkable
