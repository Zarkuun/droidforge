#include "numberselector.h"
#include "droidfirmware.h"
#include "jackvaluetabledialog.h"
#include "tuning.h"
#include "atomtext.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>

#define ONE_OVER_ZERO 99999

NumberSelector::NumberSelector(QWidget *parent)
    : AtomSubSelector{parent}
    , number(0.0)
    , isText(false)
    , numberType(ATOM_NUMBER_NUMBER)
{
    // Text entry for manual values
    labelPrefix = new QLabel(tr("1 /"), this);
    lineEdit = new QLineEdit(this);
    labelUnit = new QLabel(this);

    QHBoxLayout *valueBox = new QHBoxLayout();
    valueBox->addWidget(labelPrefix);
    valueBox->addWidget(lineEdit);
    valueBox->addWidget(labelUnit);

    // Buttons for switching between different units
    QGridLayout *buttonBox = new QGridLayout();
    buttonTable = new QPushButton(tr("Table"), this);
    buttonNumber = new QPushButton("➞ " + tr("#"), this);
    buttonVoltage = new QPushButton("➞ " + tr("V"), this);
    buttonPercentage = new QPushButton("➞ " + tr("%"), this);
    buttonOnOff = new QPushButton("➞ " + tr("□ / ▣"), this);
    buttonFraction = new QPushButton(tr("1 / X"), this);
    buttonText = new QPushButton("➞ " + tr("Text"), this);
    buttonBox->addWidget(buttonTable, 0, 0, 1, 2);
    buttonBox->addWidget(buttonNumber, 1, 0);
    buttonBox->addWidget(buttonVoltage, 1, 1);
    buttonBox->addWidget(buttonPercentage, 2, 0);
    buttonBox->addWidget(buttonOnOff, 2, 1);
    buttonBox->addWidget(buttonFraction, 3, 0, 1, 2);
    buttonBox->addWidget(buttonText, 4, 0, 1, 2);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(valueBox);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonBox);

    connect(lineEdit, &QLineEdit::textEdited, this, &NumberSelector::lineEdited);
    connect(buttonTable, &QPushButton::pressed, this, &NumberSelector::openTable);
    connect(buttonNumber, &QPushButton::pressed, this, &NumberSelector::switchToNumber);
    connect(buttonFraction, &QPushButton::pressed, this, &NumberSelector::switchToFraction);
    connect(buttonVoltage, &QPushButton::pressed, this, &NumberSelector::switchToVoltage);
    connect(buttonPercentage, &QPushButton::pressed, this, &NumberSelector::switchToPercentage);
    connect(buttonOnOff, &QPushButton::pressed, this, &NumberSelector::switchToOnOff);
    connect(buttonText, &QPushButton::pressed, this, &NumberSelector::switchToText);
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
        return atom->isNumber() || atom->isText();
}
void NumberSelector::setAtom(const Patch *, const Atom *atom)
{
    if (atom->isInvalid()) {
        setNumberType(ATOM_NUMBER_NUMBER);
        lineEdit->setText("1");
        buttonFraction->setChecked(false);
        labelPrefix->setVisible(false);
    }

    else if (atom->isText()) {
        AtomText *at = (AtomText *)atom;
        lineEdit->setText(at->getText());
        setTextType();
    }

    else {
        AtomNumber *an = (AtomNumber *)atom;
        setNumberType(an->getType());
        buttonFraction->setChecked(an->isFraction());
        labelPrefix->setVisible(an->isFraction());
        number = an->getNumber();
        if (number != 0 && an->isFraction())
            number = 1.0 / number;
        if (numberType == ATOM_NUMBER_ONOFF) {
            number = number > BOOLEAN_VALUE_THRESHOLD ? 1 : 0;
            lineEdit->setText(number ? "on" : "off");
        }
        else {
            if (numberType == ATOM_NUMBER_VOLTAGE)
                number *= 10;
            else if (numberType == ATOM_NUMBER_PERCENTAGE)
                number *= 100;
            lineEdit->setText(AtomNumber::niceNumber(number));
        }
    }
}
void NumberSelector::setAllowFraction(bool af)
{
    buttonFraction->setVisible(af);
}
void NumberSelector::setCircuitAndJack(QString c, QString j)
{
    circuit = c;
    jack = j;
    auto jackValueTable = the_firmware->jackValueTable(circuit, "inputs", jack);
    if (jackValueTable.empty()) {
        buttonTable->setVisible(false);
    }
    else {
        buttonTable->setVisible(true);
    }
}
void NumberSelector::clearAtom()
{
    setNumberType(ATOM_NUMBER_NUMBER);
    number = 0;
    lineEdit->setText("");
    buttonFraction->setChecked(false);
    labelPrefix->setVisible(false);
}
void NumberSelector::setTextType()
{
    isText = true;
    buttonNumber->setEnabled(true);
    buttonVoltage->setEnabled(true);
    buttonPercentage->setEnabled(true);
    buttonOnOff->setEnabled(true);
    buttonFraction->setEnabled(true);
    buttonText->setEnabled(false);
    labelPrefix->setText("\"");
    labelPrefix->setVisible(true);
    labelUnit->setText("\"");
}
void NumberSelector::setNumberType(atom_number_t t)
{
    numberType = t;
    isText = false;
    QString unitName;
    switch (numberType) {
    case ATOM_NUMBER_NUMBER:
        buttonNumber->setEnabled(false);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(true);
        buttonFraction->setEnabled(true);
        buttonText->setEnabled(true);
        labelPrefix->setVisible(false);
        unitName = "";
        break;

    case ATOM_NUMBER_VOLTAGE:
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(false);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(true);
        buttonFraction->setEnabled(true);
        buttonText->setEnabled(true);
        labelPrefix->setVisible(false);
        unitName = tr("V");
        break;

    case ATOM_NUMBER_PERCENTAGE:
        unitName = tr("%");
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(false);
        buttonOnOff->setEnabled(true);
        buttonFraction->setEnabled(true);
        buttonText->setEnabled(true);
        labelPrefix->setVisible(false);
        break;

    case ATOM_NUMBER_ONOFF:
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(false);
        buttonFraction->setEnabled(true);
        buttonText->setEnabled(true);
        labelPrefix->setVisible(false);
        unitName = "";
        break;

    case ATOM_NUMBER_FRACTION:
        buttonNumber->setEnabled(true);
        buttonVoltage->setEnabled(true);
        buttonPercentage->setEnabled(true);
        buttonOnOff->setEnabled(true);
        buttonFraction->setEnabled(false);
        buttonText->setEnabled(true);
        labelPrefix->setText("1 / ");
        labelPrefix->setVisible(true);
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
void NumberSelector::installFocusFilter(QWidget *w)
{
    lineEdit->installEventFilter(w);
}
Atom *NumberSelector::getAtom() const
{
    if (isText) {
        QString text = lineEdit->text();
        return new AtomText(text); // TODO: Remove non-ascii-chars
    }

    double value = number;
    if (numberType == ATOM_NUMBER_VOLTAGE)
        value /= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        value /= 100;
    else if (numberType == ATOM_NUMBER_FRACTION) {
        if (value == 0)
            value = ONE_OVER_ZERO;
        else
            value = 1.0 / value;
    }

    return new AtomNumber(value, numberType);
}
void NumberSelector::lineEdited(QString text)
{
    if (text.contains("\""))
        setTextType();

    if (isText) {
        int posBefore = lineEdit->cursorPosition();
        lineEdit->setText(AtomText::cleanText(lineEdit->text()));
        lineEdit->setCursorPosition(posBefore);
        return;
    }

    if (text.endsWith("V", Qt::CaseInsensitive)) {
        text.chop(1);
        lineEdit->setText(text);
        number = text.toDouble();
        setNumberType(ATOM_NUMBER_VOLTAGE);
    }
    else if (text.endsWith("%")) {
        text.chop(1);
        lineEdit->setText(text);
        number = text.toDouble();
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
        number = text.toDouble();
    }
}
void NumberSelector::guessNumberFromText()
{
    if (isText)
        number = lineEdit->text().toDouble();
}
void NumberSelector::switchToNumber()
{
    guessNumberFromText();
    isText = false;
    if (numberType == ATOM_NUMBER_VOLTAGE)
        number /= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number /= 100;
    else if (numberType == ATOM_NUMBER_FRACTION)
        number = (number == 0) ? ONE_OVER_ZERO : 1.0 / number;
    setNumberType(ATOM_NUMBER_NUMBER);
    lineEdit->setText(AtomNumber::niceNumber(number));
    lineEdit->setFocus();
}
void NumberSelector::switchToVoltage()
{
    guessNumberFromText();
    isText = false;
    if (numberType == ATOM_NUMBER_NUMBER ||
        numberType == ATOM_NUMBER_ONOFF)
        number *= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number /= 10;
    else if (numberType == ATOM_NUMBER_FRACTION)
        number = (number == 0) ? ONE_OVER_ZERO : 10.0 / number;
    setNumberType(ATOM_NUMBER_VOLTAGE);
    lineEdit->setText(AtomNumber::niceNumber(number));
    lineEdit->setFocus();
}
void NumberSelector::switchToPercentage()
{
    guessNumberFromText();
    isText = false;
    if (numberType == ATOM_NUMBER_NUMBER ||
        numberType == ATOM_NUMBER_ONOFF)
        number *= 100;
    else if (numberType == ATOM_NUMBER_VOLTAGE)
        number *= 10;
    else if (numberType == ATOM_NUMBER_FRACTION)
        number = (number == 0) ? ONE_OVER_ZERO : 100.0 / number;
    setNumberType(ATOM_NUMBER_PERCENTAGE);
    lineEdit->setText(AtomNumber::niceNumber(number));
    lineEdit->setFocus();
}
void NumberSelector::switchToOnOff()
{
    isText = false;
    if (numberType == ATOM_NUMBER_VOLTAGE)
        number /= 10;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number /= 100;
    else if (numberType == ATOM_NUMBER_FRACTION)
        number = (number == 0) ? ONE_OVER_ZERO : 1.0 / number;
    number = number > BOOLEAN_VALUE_THRESHOLD ? 1 : 0;
    setNumberType(ATOM_NUMBER_ONOFF);
    lineEdit->setText(number ? "on" : "off");
    lineEdit->setFocus();
}
void NumberSelector::switchToFraction()
{
    guessNumberFromText();
    isText = false;
    if (numberType == ATOM_NUMBER_NUMBER ||
        numberType == ATOM_NUMBER_ONOFF)
        number = (number == 0) ? ONE_OVER_ZERO : 1.0 / number;
    else if (numberType == ATOM_NUMBER_VOLTAGE)
        number = (number == 0) ? ONE_OVER_ZERO : 10.0 / number;
    else if (numberType == ATOM_NUMBER_PERCENTAGE)
        number = (number == 0) ? ONE_OVER_ZERO : 100.0 / number;
    setNumberType(ATOM_NUMBER_FRACTION);
    lineEdit->setText(AtomNumber::niceNumber(number));
    lineEdit->setFocus();
}
void NumberSelector::switchToText()
{
    lineEdit->setFocus();
    setTextType();
}
void NumberSelector::openTable()
{
    JackValueTableDialog jvtd(circuit, jack, this);
    int ret = jvtd.exec();
    if (ret == QDialog::Accepted) {
        number = jvtd.getSelectedValue();
        setNumberType(ATOM_NUMBER_NUMBER);
        lineEdit->setText(AtomNumber::niceNumber(number));
        lineEdit->setFocus();
    }
}
