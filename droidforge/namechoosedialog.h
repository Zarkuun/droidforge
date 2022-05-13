#ifndef NAMECHOOSEDIALOG_H
#define NAMECHOOSEDIALOG_H

#include "dialog.h"

#include <QLineEdit>
#include <QLabel>

class NameChooseDialog : public Dialog
{
    Q_OBJECT
    QLineEdit *lineEdit;
    QLabel *label;

public:
    static QString getName(const QString &title, const QString &label, const QString &oldname);

private:
    NameChooseDialog(QWidget *parent = nullptr);
};

#endif // NAMECHOOSEDIALOG_H
