#include "commentdialog.h"


#include <QVBoxLayout>
#include <QKeyEvent>

CommentDialog::CommentDialog(QWidget *parent)
    : QDialog{parent}
{
    resize(900, 600); // TODO: Größe merken. Standardgröße nach tuning.h
    setWindowTitle(tr("Edit circuit comment")); // TODO Also for jack comments

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
    if (dialog->exec() == QDialog::Accepted)
        return dialog->textEdit->toPlainText();
    else
        return c;
}

void CommentDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return && event->modifiers() | Qt::ControlModifier)
        accept();
    else
        QDialog::keyPressEvent(event);
}
