#include "sourcecodeeditor.h"

#include <QVBoxLayout>
#include <QPushButton>

SourceCodeEditor::SourceCodeEditor(const QString &originalSource, QWidget *parent)
    : Dialog{"sourcecode", parent}
    , originalSource(originalSource)
{
    setWindowTitle(tr("Edit source code"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    // Text editor
    textEdit = new QTextEdit();
    textEdit->setFontFamily("Hack"); // TODO
    textEdit->setText(originalSource);
    textEdit->moveCursor(QTextCursor::Start);
    layout->addWidget(textEdit);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(
                QDialogButtonBox::Reset |
                QDialogButtonBox::Ok |
                QDialogButtonBox::Cancel, this);
    QPushButton *resetButton = buttonBox->button(QDialogButtonBox::Reset);
    connect(resetButton, &QPushButton::pressed, this, &SourceCodeEditor::reset);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttonBox);
}

bool SourceCodeEditor::edit()
{
    return exec() == QDialog::Accepted;
}

QString SourceCodeEditor::getEditedText() const
{
    return textEdit->toPlainText();
}

void SourceCodeEditor::reset()
{
    textEdit->setText(originalSource);
}
