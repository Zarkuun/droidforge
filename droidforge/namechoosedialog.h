#ifndef NAMECHOOSEDIALOG_H
#define NAMECHOOSEDIALOG_H

#include "dialog.h"

#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>

class NameChooseDialog : public Dialog
{
    Q_OBJECT
    QLineEdit *lineEdit;
    QLabel *label;
    QDialogButtonBox *buttonBox;
    NameChooseDialog(QWidget *parent = nullptr);

public:
    static QString getName(const QString &title, const QString &label, const QString &oldname);
};

#endif // NAMECHOOSEDIALOG_H
