#ifndef CONTROLLERREMOVALDIALOG_H
#define CONTROLLERREMOVALDIALOG_H

#include "dialog.h"
#include "atomregister.h"
#include "registerlist.h"

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
        INPUT_LEAVE = 0,
        INPUT_SET_TO_ONE = 1,
        INPUT_REMOVE = 2} InputHandling;
    InputHandling inputHandling() const;

    typedef enum {
        OUTPUT_LEAVE = 0,
        OUTPUT_REMOVE = 1} OutputHandling;
    OutputHandling outputHandling() const;
};

#endif // CONTROLLERREMOVALDIALOG_H
