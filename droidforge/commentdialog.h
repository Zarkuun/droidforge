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
    static QString editComment(QString c);
    void keyPressEvent(QKeyEvent *event);
};

#endif // COMMENTDIALOG_H
