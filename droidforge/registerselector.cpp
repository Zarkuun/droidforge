#include "registerselector.h"
#include "atomregister.h"

RegisterSelector::RegisterSelector(QChar reg, QWidget *parent)
    : QGroupBox{parent}
    , defaultRegisterType(reg)
{
    lineEditNumber = new QLineEdit(this);

    valueBox = new QHBoxLayout();
    labelRegister = new QLabel();
    valueBox->addWidget(labelRegister);
    valueBox->addWidget(lineEditNumber);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(valueBox);
    mainLayout->addStretch();

    connect(lineEditNumber, &QLineEdit::textEdited, this, &RegisterSelector::lineEdited);
}


void RegisterSelector::addRegisterButton(QChar reg, QString label)
{
    QPushButton *button = new QPushButton(QString(reg) + ": " + label);
    buttons.append(button);
    mainLayout->addWidget(button);
    connect(button, &QPushButton::pressed, this, [this, reg] () {
        this->setRegisterType(reg);
    } );
}


void RegisterSelector::setRegisterType(QChar reg)
{
    registerType = reg;
    labelRegister->setText(reg);
}


void RegisterSelector::switchRegister(QChar c)
{
    for (qsizetype i=0; i<buttons.count(); i++) {
        QPushButton *b = buttons[i];
        if (b->text().startsWith(c)) {
            setRegisterType(c);
            return;
        }
    }
}



void RegisterSelector::setAtom(const AtomRegister *areg)
{
    setRegisterType(areg->getRegisterType());
    setRegisterNumber(areg->getNumber());
}

void RegisterSelector::clearAtom()
{
    setRegisterType(defaultRegisterType);
    setRegisterNumber(1);
}


AtomRegister *RegisterSelector::getAtom()
{
    unsigned number = lineEditNumber->text().toUInt();
    return new AtomRegister(registerType, 0, number);
}


void RegisterSelector::getFocus()
{
    if (lineEditNumber->text().isEmpty()) {
        lineEditNumber->setText("1");
        registerNumber = 1;
    }
    lineEditNumber->setFocus();
    lineEditNumber->selectAll();
}


void RegisterSelector::setRegisterNumber(unsigned n)
{
    registerNumber = n;
    lineEditNumber->setText(QString::number(registerNumber));
}


void RegisterSelector::lineEdited(QString text)
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
    lineEditNumber->setText(stripped);
}
