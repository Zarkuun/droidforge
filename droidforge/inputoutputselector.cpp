#include "inputoutputselector.h"
#include "atomregister.h"

#include <QHBoxLayout>
#include <QPalette>
#include <QKeyEvent>

InputOutputSelector::InputOutputSelector(QWidget *parent)
    : QGroupBox(parent)
    , registerType('I')
{
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::green);
    setPalette(pal);

    labelRegister = new QLabel("H");
    lineEdit = new QLineEdit(this);
    QHBoxLayout *valueBox = new QHBoxLayout();
    valueBox->addWidget(labelRegister);
    valueBox->addWidget(lineEdit);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(valueBox);

    addButton("I - " + tr("Input"));
    addButton("O - " + tr("Output"));
    addButton("G - " + tr("Gate"));
    addButton("N - " + tr("Normalization"));
    addButton("R - " + tr("RGB-LED"));
    addButton("X - " + tr("Special"));

    connect(lineEdit, &QLineEdit::textEdited, this, &InputOutputSelector::lineEdited);
}


void InputOutputSelector::setAtom(const AtomRegister *areg)
{
    setRegisterType(areg->getRegisterType());
    setNumber(areg->getNumber());
}


AtomRegister *InputOutputSelector::getAtom()
{
    unsigned number = lineEdit->text().toUInt();
    return new AtomRegister(registerType, 0, number);
}


void InputOutputSelector::addButton(QString label)
{
    QPushButton *button = new QPushButton(label);
    buttons.append(button);
    mainLayout->addWidget(button);
}


void InputOutputSelector::setRegisterType(QChar reg)
{
    registerType = reg;
    labelRegister->setText(reg);
}

void InputOutputSelector::switchRegister(QChar c)
{
    for (qsizetype i=0; i<buttons.count(); i++) {
        QPushButton *b = buttons[i];
        if (b->text().startsWith(c)) {
            setRegisterType(c);
            return;
        }
    }
}


void InputOutputSelector::setNumber(unsigned n)
{
    number = n;
    lineEdit->setText(QString::number(number));
}

void InputOutputSelector::lineEdited(QString text)
{
    // Extract register names and switch over to
    // different register.

    QString stripped;
    for (qsizetype i=0; i<text.size(); i++) {
        QChar c = text[i].toUpper();
        if (c.isLetter())
            switchRegister(c);
        else
            stripped.append(c);
    }
    lineEdit->setText(stripped);
}
