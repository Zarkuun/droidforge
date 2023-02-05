#include "findpanel.h"
#include "globals.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QKeyEvent>

FindPanel::FindPanel(QWidget *parent)
    : QWidget{parent}
{
    QLabel *labelFind = new QLabel(tr("Search:"));
    searchField = new QLineEdit();

    buttonPrev = new QToolButton(this);
    buttonPrev->setText("<");

    buttonNext = new QToolButton(this);
    buttonNext->setText(">");

    buttonFinished = new QToolButton(this);
    buttonFinished->setText(tr("Finished"));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(labelFind);
    mainLayout->addWidget(searchField);
    mainLayout->addWidget(buttonPrev);
    mainLayout->addWidget(buttonNext);
    mainLayout->addWidget(buttonFinished);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    connect(searchField, &QLineEdit::returnPressed, this, &FindPanel::searchForward);
    connect(buttonNext, &QToolButton::pressed, this, &FindPanel::searchForward);
    connect(buttonPrev, &QToolButton::pressed, this, &FindPanel::searchBackward);
}
void FindPanel::showEvent(QShowEvent *)
{
    searchField->selectAll();
    searchField->setFocus();
}
void FindPanel::searchForward()
{
    searchField->selectAll();
    shoutfunc;
}
void FindPanel::searchBackward()
{
    searchField->selectAll();
    shoutfunc;
}
