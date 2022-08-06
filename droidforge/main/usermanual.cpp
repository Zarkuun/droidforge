#include "os.h"
#ifdef HAVE_PDF
#include "usermanual.h"
#include "globals.h"
#include "pageselector.h"
#include "droidfirmware.h"

#include <QGridLayout>
#include <QPdfBookmarkModel>
#include <QPdfPageNavigation>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

UserManual *the_manual = 0;

UserManual::UserManual(QWidget *parent)
    : Dialog("manualviewer", parent)
    , document(this)
{
    the_manual = this;

    setCloseOnReturn(false);

    // PDF Widget
    pdfView = new QPdfView();
    pdfView->setDocument(&document);
    // pdfView->setPageMode(QPdfView::PageMode::MultiPage);
    pdfView->setZoomMode(QPdfView::ZoomMode::FitInView);
    pdfView->setFocusPolicy(Qt::NoFocus);
    pdfView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    PageSelector *pageSelector = new PageSelector(this);
    pageNavigation = pdfView->pageNavigation();
    pageNavigation->setDocument(&document);
    pageSelector->setPageNavigation(pageNavigation);

    // Buttons
    QPushButton *buttonExternal = new QPushButton(tr("Open in viewer"));
    connect(buttonExternal, &QPushButton::pressed, this, &UserManual::openExternally);

    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(pdfView, 0, 0, 1, 2);
    mainLayout->addWidget(pageSelector, 1, 0);
    mainLayout->addWidget(buttonExternal, 1, 1);

    QString filename = ":droid-manual.pdf";
    document.load(filename);
}
void UserManual::jumpToPage(unsigned nr)
{
    pageNavigation->setCurrentPage(nr-1);
}
void UserManual::openExternally()
{
    auto dirs = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    if (dirs.count() == 0)
        return;

    QDesktopServices desk;
    QDir dir = dirs[0];
    QString targetPath = dir.filePath("droid-manual-" + the_firmware->version() + ".pdf");
    QFile::copy(":droid-manual.pdf", targetPath);
    desk.openUrl("file:" + targetPath);
}
void UserManual::showCircuit(const QString &circuit)
{
    show();
    jumpToPage(the_firmware->circuitManualPage(circuit));
}

void UserManual::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        event->ignore();
}
#endif
