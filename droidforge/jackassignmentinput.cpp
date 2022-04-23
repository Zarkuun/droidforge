#include "jackassignmentinput.h"
#include "atomnumber.h"
#include "parseexception.h"

#include <QRegularExpression>
#include <QException>

JackAssignmentInput::JackAssignmentInput(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , atomA(0)
    , atomB(0)
    , atomC(0)
{
    parseInputValue(jack, valueString);
}


JackAssignmentInput::JackAssignmentInput(QString jack, QString comment)
    : JackAssignment(jack, comment)
    , atomA(0)
    , atomB(0)
    , atomC(0)
{
}


JackAssignmentInput::~JackAssignmentInput()
{
    if (atomA) delete atomA;
    if (atomB) delete atomB;
    if (atomC) delete atomC;
}


Atom *JackAssignmentInput::getAtom(unsigned n) const
{
    if (n == 0)
        return atomA;
    else if (n == 1)
        return atomB;
    else
        return atomC;
}


JackAssignment *JackAssignmentInput::clone() const
{
    JackAssignmentInput *newas = new JackAssignmentInput(jack, comment);
    newas->disabled = disabled;
    if (atomA)
        newas->atomA = atomA->clone();
    if (atomB)
        newas->atomB = atomB->clone();
    if (atomC)
        newas->atomC = atomC->clone();
    return newas;
}


QString JackAssignmentInput::valueToString() const
{
    // Do more intelligent work for a nice transformation
    // into A * B + C
    if (!atomA && !atomB && !atomC) // none defined
        return "";
    else if (atomA && !atomB && !atomC) // just A
        return atomA->toString();
    else if (!atomA && atomB && !atomC) // just B
        return atomB->toString();
    else if (!atomA && !atomB && atomC) // just C
        return atomC->toString();
    else if (atomA && atomB && !atomC) // just A and B
        return atomA->toString() + " * " + atomB->toString();
    else { // C is defined, but not just C
        QString sc, op;
        if (atomC->isNegatable()) {
            sc = atomC->toNegatedString();
            op = " - ";
        }
        else {
            sc = atomC->toString();
            op = " + ";
        }
        if (atomA && !atomB && atomC) // just A and C
            return atomA->toString() + op + sc;
        else if (!atomA && atomB && atomC) // just B and C
            return atomB->toString() + op + sc;
        else
            return atomA->toString() + " * " + atomB->toString() + op + sc;
    }
}


#define RATOMA "[^*/+-]+"
#define RATOMB "-[0-9][^*/+-]+"
#define RATOM "(" RATOMA "|" RATOMB ")"

void JackAssignmentInput::parseInputValue(QString jack, QString valueString)
{
    static QRegularExpression spaces("\\s");
    QString value = valueString.toLower();
    value.replace(spaces, "");

    static QRegularExpression form1("^" RATOM "$");
    static QRegularExpression form2("^" RATOM "[*]" RATOM "$");
    static QRegularExpression form3("^" RATOM "[*]" RATOM "[+]" RATOM "$");
    static QRegularExpression form4("^" RATOM "[+]" RATOM "$");
    static QRegularExpression form5("^" RATOM "[*]" RATOM "[-]" RATOM "$");
    static QRegularExpression form6("^" RATOM "[-]" RATOM "$");

    // TODO: Hier fehlen noch ein paar Formen. Z.B. auch was mit
    // Division

    QString a, b, c;

    QRegularExpressionMatch m;
    if ((m = form1.match(value)).hasMatch())
        a = m.captured(1);
    else if ((m = form2.match(value)).hasMatch()) {
        a = m.captured(1);
        b = m.captured(2);
    }
    else if ((m = form3.match(value)).hasMatch()) {
        a = m.captured(1);
        b = m.captured(2);
        c = m.captured(3);
    }
    else if ((m = form4.match(value)).hasMatch()) {
        a = m.captured(1);
        c = m.captured(2);
    }
    else if ((m = form5.match(value)).hasMatch()) {
        a = m.captured(1);
        b = m.captured(2);
        c = "-" + m.captured(3);
    }
    else if ((m = form6.match(value)).hasMatch()) {
        a = m.captured(1);
        c = "-" + m.captured(2);
    }
    else {
        throw ParseException("Invalid value for input " + jack + ": '" + value + "'");
    }
    atomA = parseInputAtom(a);
    atomB = parseInputAtom(b);
    atomC = parseInputAtom(c);
}


Atom *JackAssignmentInput::parseInputAtom(QString atom)
{
    if (atom == "")
        return 0;
    else if (atom[0] == '_')
        return parseCable(atom);
    else if (atom == "on" || atom == "off")
        return parseOnOff(atom);
    else if (atom[0].isDigit() || atom[0] == '-')
        return parseNumber(atom);
    else
        return parseRegister(atom);

}


Atom *JackAssignmentInput::parseOnOff(QString s)
{
    if (s == "on")
        return new AtomNumber(1.0, ATOM_NUMBER_ONOFF);
    else if (s == "off")
        return new AtomNumber(0.0, ATOM_NUMBER_ONOFF);
    else
        return 0;
}


Atom *JackAssignmentInput::parseNumber(QString s)
{
    float factor = 1.0;
    atom_number_t at = ATOM_NUMBER_NUMBER;
    if (s.endsWith("v")) {
        at = ATOM_NUMBER_VOLTAGE;
        factor = 0.1;
        s.chop(1);
    }
    else if (s.endsWith("%")) {
        at = ATOM_NUMBER_PERCENTAGE;
        factor = 0.01;
        s.chop(1);
    }
    bool ok = false;
    float number = s.toFloat(&ok);
    if (!ok)
        return 0;
    else
        return new AtomNumber(number * factor, at);
}
