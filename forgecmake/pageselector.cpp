#include "os.h"
#include "pageselector.h"
#include "globals.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPdfPageNavigator>
#include <QToolButton>

PageSelector::PageSelector(int pageCount, QWidget *parent)
    : QWidget(parent)
    , pageCount(pageCount)
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
    labelPageCount->setText(QString::number(pageCount));

    buttonNext = new QToolButton(this);
    buttonNext->setText(">");
    buttonNext->setEnabled(true);

    layout->addWidget(buttonPrev);
    layout->addWidget(lineEditPage);
    layout->addWidget(labelPageCount);
    layout->addWidget(buttonNext);
    layout->addStretch(1);
}
void PageSelector::setPageNavigator(QPdfPageNavigator *pageNav)
{
    pageNavigator = pageNav;

    connect(buttonPrev, &QToolButton::clicked, pageNavigator, &QPdfPageNavigator::back);
    connect(pageNavigator, &QPdfPageNavigator::backAvailableChanged, buttonPrev, &QToolButton::setEnabled);

    connect(pageNavigator, &QPdfPageNavigator::currentPageChanged, this, &PageSelector::onCurrentPageChanged);
    // connect(pageNavigator, &QPdfPageNavigator::pageCountChanged, this, [this](int pageCount){ labelPageCount->setText(QString::fromLatin1("/ %1").arg(pageCount)); });

    connect(lineEditPage, &QLineEdit::editingFinished, this, &PageSelector::pageNumberEdited);
    connect(lineEditPage, &KeyCaptureLineEdit::keyPressed, this, &PageSelector::handleKeyPress);

    connect(buttonNext, &QToolButton::clicked, pageNavigator, &QPdfPageNavigator::forward);
    connect(pageNavigator, &QPdfPageNavigator::forwardAvailableChanged, buttonNext, &QToolButton::setEnabled);

    onCurrentPageChanged(pageNavigator->currentPage());
}
void PageSelector::goToPage(int page, bool withHistory)
{
    shout << "goto page" << page;
    if (page > pageCount) {
        page = pageCount;
        lineEditPage->setText(QString::number(page));
    }
    else if (page < 1) {
        page = 1;
        lineEditPage->setText("1");
    }

    if (withHistory)
        pageNavigator->jump(page - 1, QPointF(0, 0), 1.0);
    else
        pageNavigator->update(page -1, QPointF(0, 0), 1.0);
}
void PageSelector::onCurrentPageChanged(int page)
{
    if (!pageNavigator)
        return;

    if (pageCount == 0)
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
    goToPage(pageNumber, true);

    if (!ok)
        onCurrentPageChanged(pageNavigator->currentPage());
    else {
        lineEditPage->selectAll();
    }
}
void PageSelector::handleKeyPress(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        shout << "left/up";
        goToPage(pageNavigator->currentPage(), false);
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Space:
        shout << "right/down/space";
        goToPage(pageNavigator->currentPage() + 2, false);
        return;
    case Qt::Key_Home:
        shout << "home";
        goToPage(1, true);
        return;
    case Qt::Key_End:
        shout << "end";
        goToPage(pageCount, true);
        return;
    }
}
