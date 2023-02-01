#ifndef HINTDIALOG_H
#define HINTDIALOG_H

#include "dialog.h"

#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>

class HintDialog : public Dialog
{
    QLabel *label;
    QCheckBox *checkbox;

    HintDialog(QWidget *parent = nullptr);
    QDialogButtonBox *buttonBox;

public:
    static void hint(const QString &id, const QString &content);

};

#endif // HINTDIALOG_H
