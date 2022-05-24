#ifndef PATCHPROPERTIESDIALOG_H
#define PATCHPROPERTIESDIALOG_H

#include "patch.h"
#include "dialog.h"

#include <QLineEdit>
#include <QTextEdit>


class PatchPropertiesDialog : public Dialog
{
    Q_OBJECT

    Patch *patch;
    QLineEdit *lineEditTitle;
    QTextEdit *textEditDescription;

public:
    static void editPatchProperties(Patch *patch);

private:
    PatchPropertiesDialog(QWidget *parent = nullptr);
    bool validateInput();
};

#endif // PATCHPROPERTIESDIALOG_H
