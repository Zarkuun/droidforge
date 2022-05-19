#ifndef CONTROLLERREMOVALDIALOG_H
#define CONTROLLERREMOVALDIALOG_H

#include "dialog.h"
#include "atomregister.h"

#include <QObject>
#include <QCheckBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QLabel>

class ControllerRemovalDialog : public Dialog
{
    QLabel *labelRemap;
    QCheckBox *checkboxRemap;
    QRadioButton *rbInputLeave;
    QRadioButton *rbInputSetToOne;
    QRadioButton *rbInputRemove;
    QRadioButton *rbOutputLeave;
    QRadioButton *rbOutputRemove;
    QDialogButtonBox *buttonBox;

public:
    ControllerRemovalDialog(QWidget *parent = nullptr);
    void setRegistersToRemap(const RegisterList &rl);
    bool shouldRemap() const;
    typedef enum {
        INPUT_LEAVE,
        INPUT_SET_TO_ONE,
        INPUT_REMOVE } InputHandling;
    InputHandling inputHandling() const;

    typedef enum {
        OUTPUT_LEAVE,
        OUTPUT_REMOVE } OutputHandling;
    OutputHandling outputHandling() const;
};

#endif // CONTROLLERREMOVALDIALOG_H
