#include "sourcecodeeditor.h"
#include "globals.h"
#include "tuning.h"

#include <QAction>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFontDatabase>
#include <QSettings>

SourceCodeEditor::SourceCodeEditor(const QString &title, const QString &originalSource, QWidget *parent, bool readonly)
    : Dialog{"sourcecode", parent}
    , originalSource(originalSource)
    , zoomLevel(0)
{
    setDefaultSize({SCD_WIDTH, SCD_HEIGHT});
    setWindowTitle(title);

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    // Text editor
    textEdit = new QTextEdit();
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    textEdit->setFontFamily(fixedFont.family());

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

    QAction *increaseFontSize = new QAction(tr("Increase font size"), this);
    increaseFontSize->setShortcuts({QKeySequence::ZoomIn, QKeySequence(tr("Ctrl+="))});
    this->addAction(increaseFontSize);
    connect(increaseFontSize, &QAction::triggered, this, &SourceCodeEditor::increaseFontSize);

    QAction *decreaseFontSize = new QAction(tr("Decrease font size"), this);
    decreaseFontSize->setShortcuts(QKeySequence::ZoomOut);
    this->addAction(decreaseFontSize);
    connect(decreaseFontSize, &QAction::triggered, this, &SourceCodeEditor::decreaseFontSize);

    QAction *normalFontSize = new QAction(tr("Normal font size"), this);
    normalFontSize->setShortcut(QKeySequence(tr("Ctrl+0")));
    this->addAction(normalFontSize);
    connect(normalFontSize, &QAction::triggered, this, &SourceCodeEditor::normalFontSize);

    QSettings settings;
    zoomLevel = settings.value("source_code_editor_zoom", 0).toInt();
    if (zoomLevel > 0)
        textEdit->zoomIn(zoomLevel);
    else if (zoomLevel < 0)
        textEdit->zoomOut(-zoomLevel);

}
void SourceCodeEditor::increaseFontSize()
{
    zoomLevel ++;
    textEdit->zoomIn(1);
    saveZoomLevel();
}
void SourceCodeEditor::decreaseFontSize()
{
    zoomLevel --;
    textEdit->zoomOut(1);
    saveZoomLevel();
}
void SourceCodeEditor::normalFontSize()
{
    if (zoomLevel > 0)
        textEdit->zoomOut(zoomLevel);
    else if (zoomLevel < 0)
        textEdit->zoomIn(-zoomLevel);
    zoomLevel = 0;
    saveZoomLevel();
}

void SourceCodeEditor::saveZoomLevel()
{
    QSettings settings;
    settings.setValue("source_code_editor_zoom", zoomLevel);
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
