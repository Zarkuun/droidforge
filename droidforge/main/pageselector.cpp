#include "os.h"
#include "pageselector.h"
#include "globals.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPdfPageNavigator>
#include <QPdfDocument>
#include <QToolButton>

PageSelector::PageSelector(QWidget *parent)
    : QWidget(parent)
    , pageNavigator(nullptr)
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

void PageSelector::setPageNavigator(QPdfPageNavigator *pageNav, QPdfDocument *doc)
{
    pageNavigator = pageNav;
    document = doc;

// FIXME
//    connect(buttonPrev, &QToolButton::clicked, pageNavigator, &QPdfPageNavigator::goToPreviousPage);
//    connect(pageNavigator, &QPdfPageNavigator::canGoToPreviousPageChanged, buttonPrev, &QToolButton::setEnabled);

    connect(pageNavigator, &QPdfPageNavigator::currentPageChanged, this, &PageSelector::onCurrentPageChanged);
    connect(document, &QPdfDocument::pageCountChanged, this, [this](int pageCount){ labelPageCount->setText(QString::fromLatin1("/ %1").arg(pageCount)); });

    connect(lineEditPage, &QLineEdit::editingFinished, this, &PageSelector::pageNumberEdited);
    connect(lineEditPage, &KeyCaptureLineEdit::keyPressed, this, &PageSelector::handleKeyPress);

// FIXME
//    connect(buttonNext, &QToolButton::clicked, pageNavigator, &QPdfPageNavigator::goToNextPage);
//    connect(pageNavigator, &QPdfPageNavigator::canGoToNextPageChanged, buttonNext, &QToolButton::setEnabled);

// FIXME
//    onCurrentPageChanged(pageNavigator->currentPage());
}

void PageSelector::onCurrentPageChanged(int page)
{
    if (!pageNavigator)
        return;

// FIXME
(void) page;
//    if (pageNavigator->pageCount() == 0)
//        lineEditPage->setText(QString::number(0));
//    else
//        lineEditPage->setText(QString::number(page + 1));
    lineEditPage->selectAll();
}

void PageSelector::pageNumberEdited()
{
    if (!pageNavigator)
        return;

    const QString text = lineEditPage->text();
// FIXME
/*
    bool ok = false;
    const int pageNumber = text.toInt(&ok);
    if (!ok)
        onCurrentPageChanged(pageNavigator->currentPage());
    else {
        pageNavigator->setCurrentPage(qBound(0, pageNumber - 1, pageNavigator->pageCount() - 1));
        lineEditPage->selectAll();
    }
*/
}


void PageSelector::handleKeyPress(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_Up:
// FIXME
//        pageNavigator->goToPreviousPage();
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Space:
// FIXME
//        pageNavigator->goToNextPage();
        return;
    case Qt::Key_Home:
// FIXME
//        pageNavigator->setCurrentPage(1);
        return;
    case Qt::Key_End:
// FIXME
//        pageNavigator->setCurrentPage(pageNavigator->pageCount());
        return;
    }
}
