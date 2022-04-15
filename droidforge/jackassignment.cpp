#include "jackassignment.h"
#include "atomcable.h"
#include "atomregister.h"
#include "atomnumber.h"

#include <QRegularExpression>

JackAssignment::JackAssignment()
    : garbled(false)
    , disabled(false)
    , atomA(0)
    , atomB(0)
    , atomC(0)
{
}


JackAssignment::~JackAssignment()
{
    if (atomA) delete atomA;
    if (atomB) delete atomB;
    if (atomC) delete atomC;
}


JackAssignment *JackAssignment::clone() const
{
    JackAssignment *newas = new JackAssignment();
    newas->jack = jack;
    newas->jackType = jackType;
    newas->garbled = garbled;
    newas->disabled = disabled;
    if (atomA)
        newas->atomA = atomA->clone();
    if (atomB)
        newas->atomB = atomB->clone();
    if (atomC)
        newas->atomC = atomC->clone();
    return newas;
}


QString JackAssignment::toString()
{
    QString s;
    if (disabled)
        s += "# ";
    s = "    " + jack + " = " + sourceString;
    if (!comment.isEmpty())
        s += " # " + comment;

    if (jackType == JACKTYPE_INPUT)
        s += "<INPUT>";
    else if (jackType == JACKTYPE_OUTPUT)
        s += "<OUTPUT>";
    else
        s += "???";

    return s;
}


void JackAssignment::parseSourceString(QString s)
{
    sourceString = s;
    if (jackType == JACKTYPE_INPUT)
        parseInputValue();
    else if (jackType == JACKTYPE_OUTPUT)
        parseOutputValue();
    // else: leave just the source string as it is
}


#define RATOMA "[^*/+-]+"
#define RATOMB "-[0-9][^*/+-]+"
#define RATOM "(" RATOMA "|" RATOMB ")"

void JackAssignment::parseInputValue()
{
    static QRegularExpression spaces("\\s");
    QString value = sourceString.toLower();
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
        qDebug() << value << " -> NO MATCH";
    }
    atomA = parseInputAtom(a);
    atomB = parseInputAtom(b);
    atomC = parseInputAtom(c);
}


Atom *JackAssignment::parseInputAtom(QString atom)
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


void JackAssignment::parseOutputValue()
{
    if (sourceString.size() > 0) {
        if (sourceString[0] == '_')
            atomA = parseCable(sourceString);
        else
            atomA = parseRegister(sourceString);
    }
    garbled = atomA == 0;
}


Atom *JackAssignment::parseCable(QString s)
{
    static QRegularExpression exp("^_[a-z][a-z0-9_]+$",  QRegularExpression::CaseInsensitiveOption);

    if (exp.match(s).hasMatch())
        return new AtomCable(s.mid(1).toUpper());
    else
        return 0;

}


Atom *JackAssignment::parseRegister(QString s)
{
    static QRegularExpression expa("^([INORX])([1-9][0-9]*)$", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression expb("^([BLPS])([1-9][0-9]*)[.]([1-9][0-9]*)$", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m;

    m = expa.match(s);
    if (m.hasMatch()) {
        QChar registerType = m.captured(1).toUpper()[0];
        unsigned number = m.captured(2).toUInt();
        return new AtomRegister(registerType, 0, number);
    }

    m = expb.match(s);
    if (m.hasMatch()) {
        QChar registerType = m.captured(1).toUpper()[0];
        unsigned controller = m.captured(2).toUInt();
        unsigned number = m.captured(3).toUInt();
        return new AtomRegister(registerType, controller, number);
    }

    return 0;
}


Atom *JackAssignment::parseOnOff(QString s)
{
    if (s == "on")
        return new AtomNumber(1.0, ATOM_NUMBER_ONOFF);
    else if (s == "off")
        return new AtomNumber(0.0, ATOM_NUMBER_ONOFF);
    else
        return 0;
}


Atom *JackAssignment::parseNumber(QString s)
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
