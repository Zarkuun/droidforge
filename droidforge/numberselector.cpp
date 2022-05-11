#include "numberselector.h"
#include "tuning.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

NumberSelector::NumberSelector(QWidget *parent)
    : AtomSubSelector{parent}
    , number(0.0)
    , numberType(ATOM_NUMBER_NUMBER)
{
    lineEdit = new QLineEdit(this);
    labelUnit = new QLabel(this);
    QHBoxLayout *valueBox = new QHBoxLayout();
    valueBox->addWidget(lineEdit);
    valueBox->addWidget(labelUnit);

    // Buttons for switching between different units
    QGridLayout *buttonBox = new QGridLayout();
    buttonNumber = new QPushButton("➞ " + tr("#"), this);
    buttonVoltage = new QPushButton("➞ " + tr("V"), this);
    buttonPercentage = new QPushButton("➞ " + tr("%"), this);
    buttonOnOff = new QPushButton("➞ " + tr("□ / ▣"), this);
    buttonBox->addWidget(buttonNumber, 0, 0);
    buttonBox->addWidget(buttonVoltage, 0, 1);
    buttonBox->addWidget(buttonPercentage, 1, 0);
    buttonBox->addWidget(buttonOnOff, 1, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(valueBox);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonBox);

    connect(lineEdit, &QLineEdit::textEdited, this, &NumberSelector::lineEdited);
    connect(buttonNumber, &QPushButton::pressed, this, &NumberSelector::switchToNumber);
    connect(buttonVoltage, &QPushButton::pressed, this, &NumberSelector::switchToVoltage);
    connect(buttonPercentage, &QPushButton::pressed, this, &NumberSelector::switchToPercentage);
    connect(buttonOnOff, &QPushButton::pressed, this, &NumberSelector::switchToOnOff);
}

bool NumberSelector::handlesAtom(const Atom *atom) const
{
    if (atom->isInvalid()) {
        QString s = atom->toString();
        if (!s.isEmpty() && s[0].isDigit())
            return true;
        else
            return false;
    }
    else
        return atom->isNumber();
}


void NumberSelector::setAtom(const Patch *, const Atom *atom)
{
    if (atom->isInvalid()) {
        setNumberType(ATOM_NUMBER_NUMBER);
        lineEdit->setText("1");
    }

    else {
        AtomNumber *an = (AtomNumber *)atom;
        setNumberType(an->getType());
        number = an->getNumber();
        if (numberType == ATOM_NUMBER_ONOFF) {
            number = number > BOOLEAN_VALUE_THRESHOLD ? 1 : 0;
            lineEdit->setText(number ? "on" : "off");
        }
        else {
            if (numberType == ATOM_NUMBER_VOLTAGE)
                number *= 10;
            else if (numberType == ATOM_NUMBER_PERCENTAGE)
                number *= 100;
            lineEdit->setText(niceNumber(number));
        }
    }
}


void NumberSelector::clearAtom()
{
    setNumberType(ATOM_NUMBER_NUMBER);
    number = 0;
    lineEdit->setText("");
}


void NumberSelector::setNumberType(atom_number_t t)
{
    numberType = t;
    QString unitName;
    switch (numberType) {
    case ATOM_NUMBER_NUMBER:
        buttonNumber->setEnabled(false);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(true);
        unitName = "";
        break;

    case ATOM_NUMBER_VOLTAGE:
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(false);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(true);
        unitName = tr("V");
        break;

    case ATOM_NUMBER_PERCENTAGE:
        unitName = tr("%");
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(false);
        buttonOnOff->setEnabled(true);
        break;

    case ATOM_NUMBER_ONOFF:
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(false);
        unitName = "";
        break;
    }
    labelUnit->setText(unitName);
}

void NumberSelector::getFocus()
{
    if (lineEdit->text().isEmpty()) {
        setNumberType(ATOM_NUMBER_NUMBER);
        lineEdit->setText("1");
        number = 1;
    }
    lineEdit->setFocus();
    lineEdit->selectAll();
}


Atom *NumberSelector::getAtom() const
{
    float value = number;
    if (numberType == ATOM_NUMBER_VOLTAGE)
        value /= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        value /= 100;
    return new AtomNumber(value, numberType);
}


QString NumberSelector::niceNumber(float v)
{
    QString s = QString::number(v, 'f', 6);
    if (!s.contains("."))
        return s; // Can this happen?

    while (s.endsWith("0"))
        s.chop(1);
    if (s.endsWith("."))
        s.chop(1);
    return s;
}


void NumberSelector::lineEdited(QString text)
{
    if (text.endsWith("V", Qt::CaseInsensitive)) {
        text.chop(1);
        lineEdit->setText(text);
        number = text.toFloat();
        setNumberType(ATOM_NUMBER_VOLTAGE);
    }
    else if (text.endsWith("%")) {
        text.chop(1);
        lineEdit->setText(text);
        number = text.toFloat();
        setNumberType(ATOM_NUMBER_PERCENTAGE);
    }
    else if (text.toLower() == "on") {
        lineEdit->setText("on");
        number = 1;
        setNumberType(ATOM_NUMBER_ONOFF);
    }
    else if (text.toLower() == "off") {
        lineEdit->setText("off");
        number = 0;
        setNumberType(ATOM_NUMBER_ONOFF);
    }
    else if (text.endsWith("#")) {
        text.chop(1);
        lineEdit->setText(text);
        setNumberType(ATOM_NUMBER_NUMBER);
    }
    else {
        number = text.toFloat();
    }
    qDebug() << "NUMBER:" << number;
}


void NumberSelector::switchToNumber()
{
    if (numberType == ATOM_NUMBER_VOLTAGE)
        number /= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number /= 100;
    setNumberType(ATOM_NUMBER_NUMBER);
    lineEdit->setText(niceNumber(number));
}


void NumberSelector::switchToVoltage()
{
    if (numberType == ATOM_NUMBER_NUMBER ||
        numberType == ATOM_NUMBER_ONOFF)
        number *= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number /= 10;
    setNumberType(ATOM_NUMBER_VOLTAGE);
    lineEdit->setText(niceNumber(number));
}


void NumberSelector::switchToPercentage()
{
    if (numberType == ATOM_NUMBER_NUMBER ||
        numberType == ATOM_NUMBER_ONOFF)
        number *= 100;
    else if (numberType == ATOM_NUMBER_VOLTAGE)
        number *= 10;
    setNumberType(ATOM_NUMBER_PERCENTAGE);
    lineEdit->setText(niceNumber(number));
}


void NumberSelector::switchToOnOff()
{
    if (numberType == ATOM_NUMBER_VOLTAGE)
        number /= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number /= 100;
    number = number > BOOLEAN_VALUE_THRESHOLD ? 1 : 0;
    setNumberType(ATOM_NUMBER_ONOFF);
    lineEdit->setText(number ? "on" : "off");
}
