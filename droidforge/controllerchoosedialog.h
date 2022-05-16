#ifndef CONTROLLERCHOOSEDIALOG_H
#define CONTROLLERCHOOSEDIALOG_H

#include "dialog.h"

#include <QDialogButtonBox>

class ControllerChooseDialog : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    ControllerChooseDialog(QWidget *parent = nullptr);

public:
    static QString chooseController();
};

#endif // CONTROLLERCHOOSEDIALOG_H
