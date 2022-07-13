#include "sourcecodeeditor.h"
#include "tuning.h"

#include <QVBoxLayout>
#include <QPushButton>

SourceCodeEditor::SourceCodeEditor(const QString &originalSource, QWidget *parent, bool readonly)
    : Dialog{"sourcecode", parent}
    , originalSource(originalSource)
{
    setDefaultSize({SCD_WIDTH, SCD_HEIGHT});
    setWindowTitle(tr("Patch source code"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    // Text editor
    textEdit = new QTextEdit();
    textEdit->setFontFamily("Hack"); // TODO
    textEdit->setText(originalSource);
    textEdit->moveCursor(QTextCursor::Start);
    textEdit->setReadOnly(readonly);
    layout->addWidget(textEdit);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(
                readonly
                ? QDialogButtonBox::Ok
                :  QDialogButtonBox::Reset | QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                this);
    if (!readonly) {
        QPushButton *resetButton = buttonBox->button(QDialogButtonBox::Reset);
        connect(resetButton, &QPushButton::pressed, this, &SourceCodeEditor::reset);
    }
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
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
