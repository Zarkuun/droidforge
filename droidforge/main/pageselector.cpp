#include "os.h"
#include "pageselector.h"
#include "globals.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtPdf/QPdfPageNavigator>
#include <QtPdf/QPdfDocument>
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

    connect(buttonPrev, &QToolButton::clicked, this, &PageSelector::goToPreviousPage);

    connect(pageNavigator, &QPdfPageNavigator::currentPageChanged, this, &PageSelector::onCurrentPageChanged);
    connect(document, &QPdfDocument::pageCountChanged, this, [this](int pageCount){ labelPageCount->setText(QString::fromLatin1("/ %1").arg(pageCount)); });

    connect(lineEditPage, &QLineEdit::editingFinished, this, &PageSelector::pageNumberEdited);
    connect(lineEditPage, &KeyCaptureLineEdit::keyPressed, this, &PageSelector::handleKeyPress);

    connect(buttonNext, &QToolButton::clicked, this, &PageSelector::goToNextPage);

    onCurrentPageChanged(pageNavigator->currentPage());
}

void PageSelector::goToPreviousPage()
{
    if (!pageNavigator)
        return;

    if(pageNavigator->currentPage() > 0)
        pageNavigator->jump(pageNavigator->currentPage() - 1, {}, pageNavigator->currentZoom());
}

void PageSelector::goToNextPage()
{
    if (!pageNavigator)
        return;

    if(pageNavigator->currentPage() < document->pageCount() - 1)
        pageNavigator->jump(pageNavigator->currentPage() + 1, {}, pageNavigator->currentZoom());
}

void PageSelector::onCurrentPageChanged(int page)
{
    if (!pageNavigator)
        return;

    buttonPrev->setEnabled(pageNavigator->currentPage() > 0);
    buttonNext->setEnabled(pageNavigator->currentPage() < document->pageCount() - 1);

    if (document->pageCount() == 0)
        lineEditPage->setText(QString::number(0));
    else
        lineEditPage->setText(QString::number(page + 1));
    lineEditPage->selectAll();
}

void PageSelector::pageNumberEdited()
{
    if (!pageNavigator)
        return;

    const QString text = lineEditPage->text();

    bool ok = false;
    const int pageNumber = text.toInt(&ok);
    if (!ok)
        onCurrentPageChanged(pageNavigator->currentPage());
    else {
        pageNavigator->jump(qBound(0, pageNumber - 1, document->pageCount() - 1), {}, pageNavigator->currentZoom());
        lineEditPage->selectAll();
    }
}


void PageSelector::handleKeyPress(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        goToPreviousPage();
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Space:
        goToNextPage();
        return;
    case Qt::Key_Home:
        pageNavigator->jump(0, {}, pageNavigator->currentZoom());
        return;
    case Qt::Key_End:
        pageNavigator->jump(document->pageCount() - 1, {}, pageNavigator->currentZoom());
        return;
    }
}
