#include "os.h"
#include "pageselector.h"
#include "globals.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPdfPageNavigation>
#include <QToolButton>

PageSelector::PageSelector(QWidget *parent)
    : QWidget(parent)
    , pageNavigation(nullptr)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    buttonPrev = new QToolButton(this);
    buttonPrev->setText("<");
    buttonPrev->setEnabled(true);

    lineEditPage = new KeyCaptureLineEdit(this);
    lineEditPage->setAlignment(Qt::AlignRight);
    lineEditPage->setMaximumWidth(50);

    labelPageCount = new QLabel(this);
    labelPageCount->setText("0");

    buttonNext = new QToolButton(this);
    buttonNext->setText(">");
    buttonNext->setEnabled(true);

    layout->addWidget(buttonPrev);
    layout->addWidget(lineEditPage);
    layout->addWidget(labelPageCount);
    layout->addWidget(buttonNext);
    layout->addStretch(1);
}

void PageSelector::setPageNavigation(QPdfPageNavigation *pageNav)
{
    pageNavigation = pageNav;

    connect(buttonPrev, &QToolButton::clicked, pageNavigation, &QPdfPageNavigation::goToPreviousPage);
    connect(pageNavigation, &QPdfPageNavigation::canGoToPreviousPageChanged, buttonPrev, &QToolButton::setEnabled);

    connect(pageNavigation, &QPdfPageNavigation::currentPageChanged, this, &PageSelector::onCurrentPageChanged);
    connect(pageNavigation, &QPdfPageNavigation::pageCountChanged, this, [this](int pageCount){ labelPageCount->setText(QString::fromLatin1("/ %1").arg(pageCount)); });

    connect(lineEditPage, &QLineEdit::editingFinished, this, &PageSelector::pageNumberEdited);
    connect(lineEditPage, &KeyCaptureLineEdit::keyPressed, this, &PageSelector::handleKeyPress);

    connect(buttonNext, &QToolButton::clicked, pageNavigation, &QPdfPageNavigation::goToNextPage);
    connect(pageNavigation, &QPdfPageNavigation::canGoToNextPageChanged, buttonNext, &QToolButton::setEnabled);

    onCurrentPageChanged(pageNavigation->currentPage());
}

void PageSelector::onCurrentPageChanged(int page)
{
    if (!pageNavigation)
        return;

    if (pageNavigation->pageCount() == 0)
        lineEditPage->setText(QString::number(0));
    else
        lineEditPage->setText(QString::number(page + 1));
    lineEditPage->selectAll();
}

void PageSelector::pageNumberEdited()
{
    if (!pageNavigation)
        return;

    const QString text = lineEditPage->text();
    bool ok = false;
    const int pageNumber = text.toInt(&ok);

    if (!ok)
        onCurrentPageChanged(pageNavigation->currentPage());
    else {
        pageNavigation->setCurrentPage(qBound(0, pageNumber - 1, pageNavigation->pageCount() - 1));
        lineEditPage->selectAll();
    }
}


void PageSelector::handleKeyPress(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        pageNavigation->goToPreviousPage();
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Space:
        pageNavigation->goToNextPage();
        return;
    case Qt::Key_Home:
        pageNavigation->setCurrentPage(1);
        return;
    case Qt::Key_End:
        pageNavigation->setCurrentPage(pageNavigation->pageCount());
        return;
    }
}
