#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include "dialog.h"

#include <QLineEdit>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>

class RenameDialog : public Dialog
{
    Q_OBJECT
    QLineEdit *lineEdit;
    QLabel *label;
    QDialogButtonBox *buttonBox;
    RenameDialog(QWidget *parent = nullptr);

public:
    static QString getRenameName(const QString &title, const QString &label, const QString &oldname);
};

#endif // RENAMEDIALOG_H
