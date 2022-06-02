#include "registerlabelwidget.h"
#include "tuning.h"

#include <QGridLayout>
#include <QLabel>


RegisterLabelWidget::RegisterLabelWidget(AtomRegister atom, const QString shortLabel, const QString longLabel, QWidget *parent)
    : QGroupBox{parent}
    , atom(atom)
{
    static QFont hackFont("Hack"); // TODO: more portable way for monospaced font. Or ship HACK.

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    QLabel *label = new QLabel(atom.toString());
    label->setFont(hackFont);
    layout->addWidget(label, 0, 0);

    lineEditShort = new QLineEdit(this);
    lineEditShort->setText(shortLabel);
    lineEditShort->setPlaceholderText(tr("short"));
    lineEditShort->setMaxLength(MAX_LENGTH_SHORTHAND);
    lineEditShort->setToolTip(tr("Short hand for the jack or control that will be "
                                 "shown directly on the module's face plate (maximum: %1 characters)").arg(MAX_LENGTH_SHORTHAND));
    layout->addWidget(lineEditShort, 0, 1);

    lineEditDescription = new QLineEdit(this);
    lineEditDescription->setText(longLabel);
    lineEditDescription->setPlaceholderText(tr("full description"));
    layout->addWidget(lineEditDescription, 1, 0, 1, 2);
}

void RegisterLabelWidget::updateLabels(RegisterLabels &labels)
{
    // TODO: validator instead of this [ and ] hack
    QString shorthand = lineEditShort->text().trimmed().replace(']', ' ').replace('[', ' ');
    QString description = lineEditDescription->text().trimmed();
    if (shorthand != "" || description != "") {
        qDebug() << "SETTING" << atom.toString() << "to" << shorthand << description;
        labels[atom] = RegisterLabel{atom, shorthand, description};
    }
    else
        labels.remove(atom);
}

void RegisterLabelWidget::select()
{
    setFocus();
    lineEditShort->selectAll();
}
