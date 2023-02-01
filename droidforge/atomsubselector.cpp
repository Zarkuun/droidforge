#include "atomsubselector.h"
#include "colorscheme.h"
#include "tuning.h"
#include "colorscheme.h"

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
        QString style = QString("QGroupBox { border: 1px solid %1; }")
                .arg(COLOR(JSEL_COLOR_LINE).name());
        setStyleSheet(style);
        emit gotSelected(this);
    }
    else if (e->type() == QEvent::FocusOut)
        setStyleSheet("");
    return false;
}
