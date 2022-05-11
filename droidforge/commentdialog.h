#ifndef COMMENTDIALOG_H
#define COMMENTDIALOG_H

#include <QTextEdit>
#include <QDialog>
#include <QDialogButtonBox>

class CommentDialog : public QDialog
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
