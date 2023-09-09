#include "findpanel.h"
#include "globals.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>

FindPanel::FindPanel(QWidget *parent)
    : QWidget{parent}
{
    label = new QLabel();
    searchField = new SearchLineEdit(this);

    buttonPrev = new QToolButton(this);
    buttonPrev->setText("<");

    buttonNext = new QToolButton(this);
    buttonNext->setText(">");

    buttonFinished = new QToolButton(this);
    buttonFinished->setText(tr("Finished"));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(label);
    mainLayout->addWidget(searchField);
    mainLayout->addWidget(buttonPrev);
    mainLayout->addWidget(buttonNext);
    mainLayout->addWidget(buttonFinished);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(searchField, &QLineEdit::returnPressed, this, &FindPanel::searchForward);
    connect(searchField, &SearchLineEdit::keyPressed, this, &FindPanel::catchKeyPress);
    connect(buttonNext, &QToolButton::pressed, this, &FindPanel::searchForward);
    connect(buttonPrev, &QToolButton::pressed, this, &FindPanel::searchBackward);
    connect(buttonFinished, &QToolButton::pressed, this, &FindPanel::finished);
}
void FindPanel::showEvent(QShowEvent *)
{
    searchField->selectAll();
    searchField->setFocus();
    label->setText(tr("Search:"));

}
void FindPanel::doSearch(int direction)
{
    searchField->selectAll();
    QString text = searchField->text().trimmed();
    if (text != "")
        emit search(text, direction);
}

void FindPanel::updateSearchStats(unsigned pos, unsigned count)
{
    if (!count)
        label->setText(tr("Search (no hit)"));
    else if (!pos)
        label->setText(tr("Search (%1 hits)").arg(count));
    else
        label->setText(tr("Search (%1/%2)").arg(pos).arg(count));
}
void FindPanel::searchForward()
{
    doSearch(1);
}
void FindPanel::searchBackward()
{
    doSearch(-1);
}
void FindPanel::catchKeyPress(QKeyEvent *event)
{
    emit keyCaptured(event);
}
void FindPanel::finished()
{
    setDisabled(true);
    hide();
}
