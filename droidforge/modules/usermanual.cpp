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
    // QPushButton *button = new QPushButton(tr("Close"));
    // connect(button, &QPushButton::pressed, this, &QDialog::close);

    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(pdfView, 0, 0, 1, 2);
    mainLayout->addWidget(pageSelector, 1, 0);

    QString filename = ":droid-manual.pdf";
    document.load(filename);
}
void UserManual::jumpToPage(unsigned nr)
{
    pageNavigation->setCurrentPage(nr-1);
}
void UserManual::showCircuit(const QString &circuit)
{
    show();
    jumpToPage(the_firmware->circuitManualPage(circuit));
}
