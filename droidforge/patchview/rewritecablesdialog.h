#ifndef REWRITECABLESDIALOG_H
#define REWRITECABLESDIALOG_H

#include "dialog.h"
#include "cableselectorlineedit.h"

#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>

class RewriteCablesDialog : public Dialog
{
    Q_OBJECT
    CableSelectorLineEdit *lineEditRemove;
    CableSelectorLineEdit *lineEditInsert;

    QRadioButton *rbSearchReplace;
    QRadioButton *rbAddPrefix;
    QRadioButton *rbAddSuffix;
    QCheckBox *cbUseRegexReplace;

    QLabel *lSearch;
    QLabel *lReplace;
    QLabel *lSuffix;
    QLabel *lPrefix;

public:
    RewriteCablesDialog(QWidget *parent = nullptr);
    QString validateInput();
    QString getRemoved() const;
    QString getInserted() const;
    typedef enum { SEARCH, PREFIX, SUFFIX } mode_t;
    mode_t getMode() const;

private slots:
    void updateMode();
};

#endif // REWRITECABLESDIALOG_H
