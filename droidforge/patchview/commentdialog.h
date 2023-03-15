#ifndef COMMENTDIALOG_H
#define COMMENTDIALOG_H

#include "dialog.h"

#include <QTextEdit>
#include <QDialogButtonBox>

class CommentDialog : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;

public:
    CommentDialog(QWidget *parent = nullptr);
    static QString editComment(const QString &windowTitle, QString c);
};

#endif // COMMENTDIALOG_H
