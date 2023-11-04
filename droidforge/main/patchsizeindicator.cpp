#include "patchsizeindicator.h"
#include "colorscheme.h"
#include "droidfirmware.h"
#include "updatehub.h"
#include "globals.h"
#include "mainwindow.h"

#define MI_WIDTH 200

PatchSizeIndicator::PatchSizeIndicator(MainWindow *mainWindow, PatchEditEngine *patch)
    : QWidget{mainWindow}
    , PatchView(patch)
    , mainWindow(mainWindow)
    , memoryNeeded(0)
    , memoryAvailable(1)
    , patchSize(0)
{
    setMinimumWidth(MI_WIDTH);
    setMaximumWidth(MI_WIDTH);

    connect(mainWindow->theHub(), &UpdateHub::patchModified, this, &PatchSizeIndicator::updateStatus);
    connect(mainWindow->theHub(), &UpdateHub::sectionSwitched, this, &PatchSizeIndicator::updateStatus);
}
void PatchSizeIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), COLOR(COLOR_STATUSBAR_BACKGROUND));

    // 1. Percentage of RAM
    float usedRAM = float(memoryNeeded) / float(memoryAvailable);
    unsigned percRAM = 100 * usedRAM;

    float usedSize = float(patchSize) / float(MAX_DROID_INI);
    unsigned percSize = 100 * usedSize;

    QRectF barRect = rect().adjusted(2, 1, -2, -1);

    if (memoryNeeded <= memoryAvailable && patchSize <= MAX_DROID_INI) {
        painter.fillRect(barRect, COLOR(PSI_COLOR_BAR));
        QRectF barRectUsed(barRect.left(), barRect.top(), qMin(1.0, qMax(usedRAM, usedSize)) * barRect.width(), barRect.height());
        painter.fillRect(barRectUsed, COLOR(PSI_COLOR_BAR_USED));
    }
    else {
        painter.fillRect(barRect, COLOR(PSI_COLOR_BAR_FULL));
    }
    painter.setPen(COLOR(PSI_COLOR_BAR_BORDER));
    painter.drawRect(barRect);

    QRectF textRect = rect();
    QString text;
    if (percRAM >= percSize)
        text = tr("%1% of RAM").arg(percRAM);
    else
        text = tr("%1% of size").arg(percSize);
    painter.setPen(COLOR(PSI_COLOR_TEXT));
    painter.drawText(textRect, text, Qt::AlignVCenter | Qt::AlignCenter);
}
void PatchSizeIndicator::updateStatus()
{
    QStringList breakdown;
    memoryAvailable = the_firmware->availableMemory(patch->typeOfMaster());
    memoryNeeded = patch->memoryFootprint(breakdown);
    QString tooltipRAM = tr("Your patch needs %1 bytes of RAM.\n").arg(memoryNeeded);
    for (auto& hint: breakdown)
        tooltipRAM += " * " + hint + "\n";

    if (memoryNeeded <= memoryAvailable) {
        unsigned perc = memoryNeeded * 100 / memoryAvailable;
        tooltipRAM += tr("This is %1% of the available RAM. You have %2 bytes left.\n").arg(perc).arg(memoryAvailable - memoryNeeded);
    }
    else {
        tooltipRAM += tr("This is %1 bytes more than there is available! "
                            "Try to remove some circuits.\n").arg(memoryNeeded - memoryAvailable);
    }

    unsigned sectionRam = patch->currentSection()->memoryFootprint();
    QString tooltipSection = tr("The section '%1' needs %2 bytes.")
            .arg(patch->currentSection()->getNonemptyTitle())
            .arg(sectionRam);
    tooltipSection += " " + tr("This is %1% of the patch size and %2% of the available RAM.")
            .arg(100 * sectionRam / memoryNeeded)
            .arg(100 * sectionRam / memoryAvailable);

    patchSize = patch->toCompressed().size();
    QString tooltipSize = tr("Your patch size is %1 bytes.").arg(patchSize);
    if (patchSize > MAX_DROID_INI)
        tooltipSize += " " + tr("That's more than the allowed %1! Check compression in the preferences.").arg(MAX_DROID_INI);
    else
        tooltipSize += " " + tr("That is %1% of the maximum of %2.")
                             .arg(QString::number((unsigned)(patchSize * 100 / MAX_DROID_INI)))
                             .arg(MAX_DROID_INI);

    setToolTip(tooltipRAM + "\n" + tooltipSection + "\n" + tooltipSize);
    update();
}
