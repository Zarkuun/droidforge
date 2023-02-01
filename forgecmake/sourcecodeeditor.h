#ifndef SOURCECODEEDITOR_H
#define SOURCECODEEDITOR_H

#include <dialog.h>

#include <QTextEdit>
#include <QDialogButtonBox>

class SourceCodeEditor : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QString originalSource;

public:
    SourceCodeEditor(const QString &title, const QString &originalSource, QWidget *parent, bool readonly);
    bool edit();
    void showReadOnly();
    QString getEditedText() const;

private slots:
    void reset();
};

#endif // SOURCECODEEDITOR_H
