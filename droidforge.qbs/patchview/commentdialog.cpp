#include "commentdialog.h"
#include "tuning.h"

#include <QVBoxLayout>

CommentDialog::CommentDialog(QWidget *parent)
    : Dialog{"comment", parent}
{
    setWindowTitle(tr("Edit circuit comment"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    // Text editor
    textEdit = new QTextEdit();
    layout->addWidget(textEdit);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}


QString CommentDialog::editComment(QString c)
{
    static CommentDialog *dialog = 0;
    if (!dialog)
        dialog = new CommentDialog();

    dialog->textEdit->setText(c);
    dialog->textEdit->moveCursor(QTextCursor::End);
    if (dialog->exec() == QDialog::Accepted)
        return dialog->textEdit->toPlainText().trimmed();
    else
        return c;
}