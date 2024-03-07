#ifndef SOURCECODEEDITOR_H
#define SOURCECODEEDITOR_H

#include "dialog.h"

#include <QTextEdit>
#include <QDialogButtonBox>

class SourceCodeEditor : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;
    QString originalSource;
    int zoomLevel;

public:
    SourceCodeEditor(const QString &title, const QString &originalSource, QWidget *parent, bool readonly);
    void addButton(QPushButton *button);
    void updateContent(const QString &content);
    bool edit();
    void showReadOnly();
    QString getEditedText() const;

private slots:
    void reset();
    void increaseFontSize();
    void decreaseFontSize();
    void normalFontSize();

private:
    void saveZoomLevel();
};

#endif // SOURCECODEEDITOR_H
