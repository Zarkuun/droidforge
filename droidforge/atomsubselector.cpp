#include "atomsubselector.h"
#include "tuning.h"

#include <QMouseEvent>


AtomSubSelector::AtomSubSelector(QWidget *parent)
    : QGroupBox{parent}
{
    setStyleSheet("QGroupBox:enabled { border: 1px solid yellow; }");
    setFixedWidth(ASEL_SUBSELECTOR_WIDTH);
}
