#ifndef PATCHPROPERTIESDIALOG_H
#define PATCHPROPERTIESDIALOG_H

#include "patcheditengine.h"
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
    static bool editPatchProperties(PatchEditEngine *patch);

private:
    PatchPropertiesDialog(QWidget *parent = nullptr);
    bool validateInput();
};

#endif // PATCHPROPERTIESDIALOG_H
