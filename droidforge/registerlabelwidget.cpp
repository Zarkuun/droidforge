#include "registerlabelwidget.h"
#include "tuning.h"

#include <QGridLayout>
#include <QLabel>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

RegisterLabelWidget::RegisterLabelWidget(AtomRegister atom, const QString shortLabel, const QString longLabel, QWidget *parent)
    : QGroupBox{parent}
    , atom(atom)
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    QLabel *label = new QLabel(atom.toString());
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    fixedFont.setPointSize(12);

    label->setFont(fixedFont);
    layout->addWidget(label, 0, 0);

    QRegularExpression re("[^][]*");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(re, this);

    lineEditShort = new QLineEdit(this);
    lineEditShort->setText(shortLabel);
    lineEditShort->setValidator(validator);
    lineEditShort->setPlaceholderText(tr("short"));
    lineEditShort->setMaxLength(MAX_LENGTH_SHORTHAND);
    lineEditShort->setToolTip(tr("Short hand for the jack or control that will be "
                                 "shown directly on the module's face plate (maximum: %1 characters)").arg(MAX_LENGTH_SHORTHAND));
    layout->addWidget(lineEditShort, 0, 1);

    lineEditDescription = new QLineEdit(this);
    lineEditDescription->setText(longLabel);
    lineEditDescription->setValidator(validator);
    lineEditDescription->setPlaceholderText(tr("full description"));
    layout->addWidget(lineEditDescription, 1, 0, 1, 2);

    setMinimumWidth(RLD_MINIMUM_LABEL_WIDTH);
}

void RegisterLabelWidget::updateLabels(RegisterLabels &labels)
{
    QString shorthand = lineEditShort->text().trimmed();
    QString description = lineEditDescription->text().trimmed();
    if (shorthand != "" || description != "")
        labels[atom] = RegisterLabel{shorthand, description};
    else
        labels.remove(atom);
}

void RegisterLabelWidget::select()
{
    setFocus();
    lineEditShort->selectAll();
}

void RegisterLabelWidget::clear()
{
    lineEditShort->clear();
    lineEditDescription->clear();
}
