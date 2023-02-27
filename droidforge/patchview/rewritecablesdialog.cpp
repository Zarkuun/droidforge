#include "rewritecablesdialog.h"
#include "globals.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QRegularExpression>

RewriteCablesDialog::RewriteCablesDialog(QWidget *parent)
    : Dialog{"rewrite_cables", parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QLabel *label = new QLabel(tr("This will rewrite the names of "
                                  "all (or all selected) internal patch cables."));
    mainLayout->addWidget(label);

    rbSearchReplace = new QRadioButton(tr("Search and replace texts in cable names"));
    rbSearchReplace->setChecked(true);
    rbAddPrefix = new QRadioButton(tr("Add a prefix to each cable name"));
    rbAddSuffix = new QRadioButton(tr("Append a suffix to each cable name"));
    mainLayout->addWidget(rbSearchReplace);
    mainLayout->addWidget(rbAddPrefix);
    mainLayout->addWidget(rbAddSuffix);

    QHBoxLayout *lineLayout = new QHBoxLayout();
    lSearch = new QLabel(tr("Search:"));
    lineEditRemove = new CableSelectorLineEdit();
    lReplace = new QLabel(tr("Replace with:"));
    lSuffix = new QLabel(tr("Suffix to add:"));
    lSuffix->setVisible(false);
    lPrefix = new QLabel(tr("Prefix to add:"));
    lPrefix->setVisible(false);
    lineEditInsert = new CableSelectorLineEdit();

    lineLayout->addWidget(lSearch);
    lineLayout->addWidget(lineEditRemove);
    lineLayout->addWidget(lReplace);
    lineLayout->addWidget(lSuffix);
    lineLayout->addWidget(lPrefix);
    lineLayout->addWidget(lineEditInsert);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(lineLayout);
    connect(rbSearchReplace, &QRadioButton::clicked, this, &RewriteCablesDialog::updateMode);
    connect(rbAddPrefix, &QRadioButton::clicked, this, &RewriteCablesDialog::updateMode);
    connect(rbAddSuffix, &QRadioButton::clicked, this, &RewriteCablesDialog::updateMode);

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

}
void RewriteCablesDialog::updateMode()
{
    lSearch->setVisible(rbSearchReplace->isChecked());
    lReplace->setVisible(rbSearchReplace->isChecked());
    lineEditRemove->setVisible(rbSearchReplace->isChecked());
    lPrefix->setVisible(rbAddPrefix->isChecked());
    lSuffix->setVisible(rbAddSuffix->isChecked());
}
QString RewriteCablesDialog::validateInput()
{
    static QRegularExpression reCablePart("^[a-zA-Z0-9_]*$");
    QString remove = lineEditRemove->text();
    QString insert = lineEditInsert->text();
    QRegularExpressionMatch m = reCablePart.match(insert);
    if (!m.hasMatch())
        return tr("Cable names must only consist of 0..9, A...Z and _");

    if (rbAddPrefix->isChecked() || rbAddSuffix->isChecked()) {
        if (insert == "")
            return tr("Please enter the text to add");
    }
    else if (rbSearchReplace->isChecked()) {
        if (remove == "")
            return tr("Please specify something to be replaced");
    }
    return "";
}
QString RewriteCablesDialog::getRemoved() const
{
    return lineEditRemove->text().trimmed();
}

QString RewriteCablesDialog::getInserted() const
{
    return lineEditInsert->text().trimmed();
}
RewriteCablesDialog::mode_t RewriteCablesDialog::getMode() const
{
    if (rbAddPrefix->isChecked())
        return PREFIX;
    else if (rbAddSuffix->isChecked())
        return SUFFIX;
    else
        return SEARCH;
}
