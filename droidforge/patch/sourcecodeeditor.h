#ifndef SOURCECODEEDITOR_H
#define SOURCECODEEDITOR_H

#include <dialog.h>

#include <QTextEdit>
#include <QDialogButtonBox>

class SourceCodeEditor : public Dialog
{
    Q_OBJECT

    // TODO: Größe irgendwie vorgeben?

    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QString originalSource;

public:
    SourceCodeEditor(const QString &originalSource, QWidget *parent = nullptr);
    bool edit();
    QString getEditedText() const;

private slots:
    void reset();
};

#endif // SOURCECODEEDITOR_H