#ifndef ATOMSELECTOR_H
#define ATOMSELECTOR_H

#include "atom.h"
#include "controlselector.h"
#include "numberselector.h"
#include "inputoutputselector.h"

#include <QWidget>

typedef enum {
    SELECT_NUMBER,
    SELECT_INPUT_OUTPUT,
    SELECT_CONTROL,
    SELECT_CABLE,
} select_t;

class AtomSelector : public QWidget
{
    Q_OBJECT
    QPushButton *buttonNumber;
    QPushButton *buttonInputOutput;
    QPushButton *buttonControl;
    QPushButton *buttonCable;
    NumberSelector *numberSelector;
    InputOutputSelector *inputOutputSelector;
    ControlSelector *controlSelector;
    select_t selectType;

public:
    explicit AtomSelector(QWidget *parent = nullptr);
    void setAtom(const Atom *atom);
    Atom *getAtom();

private:
    void setSelectType(select_t sel);

private slots:
    void switchToNumber();
    void switchToInputOutput();
    void switchToControl();
    void switchToCable();
};

#endif // ATOMSELECTOR_H
