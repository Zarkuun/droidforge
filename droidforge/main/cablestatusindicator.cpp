#include "cablestatusindicator.h"
#include "cablecolorizer.h"
#include "tuning.h"

#include <QDebug>

CableStatusIndicator::CableStatusIndicator(QWidget *parent)
    : QWidget{parent}
{
    resize(400, 100);
    setMinimumWidth(CSD_WIDTH);
    setMaximumWidth(CSD_WIDTH);
    qDebug("Mich gibt's");
}

void CableStatusIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), CSD_BACKGROUND_COLOR);

    if (cableName == "")
        return;

    float imgHeight = height() - 2 * CSD_IMAGE_MARGIN;
    float imgWidth =  imgHeight * the_cable_colorizer->imageAspect();
    QRectF leftPlugRect(CSD_SIDE_PADDING, CSD_IMAGE_MARGIN, imgWidth, imgHeight);
    QRectF rightPlugRect(width() - CSD_SIDE_PADDING - imgWidth, CSD_IMAGE_MARGIN, imgWidth, imgHeight);

    // First paint the "cable"
    QPen pen;
    pen.setColor(QColor(40, 40, 40));
    pen.setWidth(imgHeight * CSD_CABLE_THICKNESS);
    painter.setPen(pen);
    painter.drawLine(leftPlugRect.right(), height()/2, rightPlugRect.left(), height()/2);

    QPen hilitePen;
    hilitePen.setColor(QColor(80, 80, 80));
    hilitePen.setWidth(1);
    painter.setPen(hilitePen);
    painter.drawLine(leftPlugRect.right(), height()/2 - 1, rightPlugRect.left(), height()/2 - 1);

    const QImage *plugImage = the_cable_colorizer->imageForCable(cableName);
    QImage mirroredPlugImage = plugImage->mirrored(true, false);
    painter.drawImage(rightPlugRect, *plugImage);
    painter.drawImage(leftPlugRect, mirroredPlugImage);

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
