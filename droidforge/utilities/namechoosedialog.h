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
    bool forceCableName;

public:
    static QString getReName(const QString &title, const QString &label, QString oldname = "", bool forceCableName = false);
    static QString getNewName(const QString &title, const QString &label, QString oldname = "", bool forceCableName = false);
    void setForceCableName(bool f) { forceCableName = f; };

private:
    static QString getName(const QString &title, const QString &label, QString oldname, bool forceCableName, bool rename);

private slots:
    void changeText(const QString &t);

private:
    NameChooseDialog(QWidget *parent = nullptr);
};

#endif // NAMECHOOSEDIALOG_H
