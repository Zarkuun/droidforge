#include "registerselector.h"
#include "atomregister.h"
#include "tuning.h"

#include <QRegularExpressionValidator>

RegisterSelector::RegisterSelector(bool isControl, register_type_t reg, QString regs, QWidget *parent)
    : AtomSubSelector{parent}
    , isControl(isControl)
    , defaultRegisterType(reg)
    , allowedRegisters(regs)
{
    valueBox = new QHBoxLayout();
    labelRegister = new QLabel();
    valueBox->addWidget(labelRegister);
    lineEditController = new QLineEdit(this);
    lineEditController->setValidator(validator());
    valueBox->addWidget(lineEditController);
    QLabel *labelDot = new QLabel(".");
    valueBox->addWidget(labelDot);
    lineEditNumber = new QLineEdit(this);
    lineEditNumber->setValidator(validator());
    valueBox->addWidget(lineEditNumber);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(valueBox);
    mainLayout->addStretch();

    lineEditController->setVisible(isControl);
    labelDot->setVisible(isControl);

    connect(lineEditNumber, &QLineEdit::textEdited, this, &RegisterSelector::lineNumberEdited);
    connect(lineEditController, &QLineEdit::textEdited, this, &RegisterSelector::lineControllerEdited);
}


void RegisterSelector::addRegisterButton(register_type_t reg, QString label)
{
    QPushButton *button = new QPushButton(QString(reg) + ": " + label);
    buttons.append(button);
    mainLayout->addWidget(button);
    connect(button, &QPushButton::pressed, this, [this, reg] () {
        this->setRegisterType(reg);
    } );
}


void RegisterSelector::setRegisterType(register_type_t reg)
{
    registerType = reg;
    labelRegister->setText(QChar(reg));
}


void RegisterSelector::switchRegister(register_type_t c)
{
    for (qsizetype i=0; i<buttons.count(); i++) {
        QPushButton *b = buttons[i];
        if (b->text().startsWith(QChar(c))) {
            setRegisterType(c);
            return;
        }
    }
}

void RegisterSelector::setControllerNumber(unsigned controller)
{
    controllerNumber = controller;
    lineEditController->setText(QString::number(controllerNumber));
}


void RegisterSelector::setRegisterNumber(unsigned n)
{
    registerNumber = n;
    lineEditNumber->setText(QString::number(registerNumber));
}

void RegisterSelector::setAtom(const Patch *, const Atom *atom)
{
    if (atom->isInvalid()) {
        setRegisterType(REGISTER_INPUT);
        setRegisterNumber(1);
        setControllerNumber(0);
    }
    else {
        const AtomRegister *areg = (const AtomRegister *)atom;
        setRegisterType(areg->getRegisterType());
        setRegisterNumber(areg->getNumber());
        setControllerNumber(areg->getController());
    }
}


void RegisterSelector::clearAtom()
{
    setRegisterType(defaultRegisterType);
    setControllerNumber(isControl ? 1 : 0);
    setRegisterNumber(1);
}


Atom *RegisterSelector::getAtom() const
{
    unsigned number = lineEditNumber->text().toUInt();
    unsigned controller = isControl ? lineEditController->text().toUInt() : 0;
    return new AtomRegister(registerType, controller, number);
}

void RegisterSelector::installFocusFilter(QWidget *w)
{
    lineEditController->installEventFilter(w);
    lineEditNumber->installEventFilter(w);
}


void RegisterSelector::getFocus()
{
    if (lineEditNumber->text().isEmpty()) {
        lineEditNumber->setText("1");
        registerNumber = 1;
    }

    if (isControl)
    {
        lineEditController->setFocus();
        lineEditController->selectAll();
    }
    else {
        lineEditNumber->setFocus();
        lineEditNumber->selectAll();
    }
}

void RegisterSelector::stripExtraChars(QLineEdit *edit)
{
    // Extract register names and switch over to
    // different register.

    QString text = edit->text();
    QString stripped;
    for (qsizetype i=0; i<text.size(); i++) {
        register_type_t c = text[i].toUpper().toLatin1();
        if (isValidRegister(c)) {
            switchRegister(c);
            if (isControl) {
                lineEditController->setFocus();
                lineEditController->selectAll();
            }
        }
        else if (c == '.') {
            lineEditNumber->setFocus();
            lineEditNumber->selectAll();
        }
        else
            stripped.append(c);
    }
    edit->setText(stripped);
}

bool RegisterSelector::isValidRegister(register_type_t c)
{
    for (qsizetype i=0; i<buttons.count(); i++) {
        if (buttons[i]->text().startsWith(QChar(c)))
            return true;
    }
    return false;
}

QValidator *RegisterSelector::validator() const
{
    QString regex = "[0-9" + allowedRegisters + allowedRegisters.toLower();
    if (isControl)
        regex += ".";
    regex += "]+";
    QRegularExpression re(regex);
    return new QRegularExpressionValidator(re);
}


void RegisterSelector::lineNumberEdited(QString)
{
    stripExtraChars(lineEditNumber);
}

void RegisterSelector::lineControllerEdited(QString)
{
    stripExtraChars(lineEditController);
}
