#include "atomsubselector.h"
#include "tuning.h"

#include <QMouseEvent>
#include <QFocusEvent>


AtomSubSelector::AtomSubSelector(QWidget *parent)
    : QGroupBox{parent}
{
    setFixedWidth(ASEL_SUBSELECTOR_WIDTH);
}

bool AtomSubSelector::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::FocusIn) {
        setStyleSheet("QGroupBox { border: 1px solid yellow; }");
        emit gotSelected(this);
    }
    else if (e->type() == QEvent::FocusOut)
        setStyleSheet("");
    return false;
}
