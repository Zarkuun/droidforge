#include "inputoutputselector.h"
#include "atomregister.h"

#include <QHBoxLayout>
#include <QPalette>
#include <QKeyEvent>

InputOutputSelector::InputOutputSelector(QWidget *parent)
    : RegisterSelector(false, 'I', "IOGNRX", parent)
{
    addRegisterButton('I', tr("Input"));
    addRegisterButton('O', tr("Output"));
    addRegisterButton('G', tr("Gate"));
    addRegisterButton('N', tr("Normalization"));
    addRegisterButton('R', tr("RGB-LED"));
    addRegisterButton('X', tr("Special"));
}
