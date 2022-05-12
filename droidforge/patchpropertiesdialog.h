#ifndef PATCHPROPERTIESDIALOG_H
#define PATCHPROPERTIESDIALOG_H

#include "patch.h"
#include "dialog.h"

namespace Ui {
class PatchPropertiesDialog;
}

class PatchPropertiesDialog : public Dialog
{
    Q_OBJECT
    Patch *patch;

public:
    explicit PatchPropertiesDialog(Patch *patch,
            QWidget *parent = nullptr);
    ~PatchPropertiesDialog();

private:
    Ui::PatchPropertiesDialog *ui;
    void done(int r);
    bool validateInput();

private slots:
    void apply();
};

#endif // PATCHPROPERTIESDIALOG_H
