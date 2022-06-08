#include "jackassignmentinput.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomnumber.h"
#include "globals.h"
#include "parseexception.h"

#include <QRegularExpression>
#include <QException>
#include <QCoreApplication>

#define tr(s) QCoreApplication::translate("Patch", s)

JackAssignmentInput::JackAssignmentInput(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , atomA(0)
    , atomB(0)
    , atomC(0)
{
    parseInputExpression(jack, valueString);
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

const Atom *JackAssignmentInput::atomAt(int column) const
{
    return getAtom(column - 1); // column is 1, 2 or 3
}

Atom *JackAssignmentInput::atomAt(int column)
{
    return getAtom(column - 1); // column is 1, 2 or 3
}


void JackAssignmentInput::replaceAtom(int column, Atom *newAtom)
{
    // TODO: Sollten wir nicht so ein Array der Atome machen, anstelle
    // von a, b und c?
    Atom *old;
    if (column == 1) {
        old = atomA;
        atomA = newAtom;
    }
    else if (column == 2) {
        old = atomB;
        atomB = newAtom;
    }
    else {
        old = atomC;
        atomC = newAtom;
    }
    if (old)
        delete old;
}

void JackAssignmentInput::removeRegisterReferences(RegisterList &rl, int ih, int)
{
    if (ih == 0) // INPUT_LEAVE
        return;

    // Atom A
    if (isInRegisterList(rl, atomA)) {
        delete atomA;
        if (ih == 1) // INPUT_SET_TO_ONE
            atomA = new AtomNumber(1.0, ATOM_NUMBER_NUMBER, false);
        else
            atomA = 0;
    }

    if (isInRegisterList(rl, atomB)) {
        delete atomB;
        atomB = 0;
    }

    if (isInRegisterList(rl, atomC)) {
        delete atomC;
        atomC = 0;
    }
}


bool JackAssignmentInput::isInRegisterList(const RegisterList &rl, Atom *atom)
{
    if (!atom)
        return false;
    if (!atom->isRegister())
        return false;

    return rl.contains(*(AtomRegister *)atom);
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

void JackAssignmentInput::parseExpression(const QString &expression)
{
    parseInputExpression(jack, expression);
}

QString JackAssignmentInput::valueToString() const
{
    QString bOperator;
    if (atomB && atomB->isNumber() && ((AtomNumber *)atomB)->isFraction())
        bOperator = " / ";
    else
        bOperator = " * ";

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
        return atomA->toString() + bOperator + atomB->toString();
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
            return atomA->toString() + bOperator + atomB->toString() + op + sc;
    }
}

void JackAssignmentInput::collectCables(QStringList &cables) const
{
    // TODO: Endlich atoms als Liste!!
    if (atomA && atomA->isCable())
        cables.append(((AtomCable *)atomA)->getCable());
    if (atomB && atomB->isCable())
        cables.append(((AtomCable *)atomB)->getCable());
    if (atomC && atomC->isCable())
        cables.append(((AtomCable *)atomC)->getCable());
}

void JackAssignmentInput::findCableConnections(const QString &cable, int &asInput, int &) const
{
    if (atomA && atomA->isCable() && ((AtomCable *)atomA)->getCable() == cable)
        asInput ++;
    if (atomB && atomB->isCable() && ((AtomCable *)atomB)->getCable() == cable)
        asInput ++;
    if (atomC && atomC->isCable() && ((AtomCable *)atomC)->getCable() == cable)
        asInput ++;
}


// TODO: Kann man da nicht saubere REGEXe für die drei verschiedenen Arten
// von Atom machen? Zahlen, Register, Kabel. Und dann das hier sauber verschachteln.
// das mit dem [^*/+-] scheint mir ein ziemlich wackliger hack zu sein.
#define RATOMA "[^*/+-]+"
#define RATOMB "-[0-9][^*/+-]+"
#define RATOM "(" RATOMA "|" RATOMB ")"
#define RNUMBER "(-?([0-9]+[.])?[0-9]+)"

// TODO: -1*_postfader_input_tb+1 geht nicht

void JackAssignmentInput::parseInputExpression(QString, QString valueString)
{
    static QRegularExpression spaces("\\s");
    QString value = valueString.toLower();
    value.replace(spaces, "");

    static QRegularExpression form0("^$");
    static QRegularExpression form1("^" RATOM "$");
    static QRegularExpression form2("^" RATOM "[*]" RATOM "$");
    static QRegularExpression form3("^" RATOM "[*]" RATOM "[+]" RATOM "$");
    static QRegularExpression form4("^" RATOM "[+]" RATOM "$");
    static QRegularExpression form5("^" RATOM "[*]" RATOM "[-]" RATOM "$");
    static QRegularExpression form6("^" RATOM "[-]" RATOM "$");
    static QRegularExpression form7("^" RATOM "[/]" RNUMBER "$");
    static QRegularExpression form8("^" RATOM "[/]" RNUMBER "[+]" RATOM "$");
    static QRegularExpression form9("^" RATOM "[/]" RNUMBER "[-]" RNUMBER "$");

    // TODO: Hier fehlen noch ein paar Formen. Z.B. auch was mit
    // Division

    QString a, b, c;

    QRegularExpressionMatch m;
    if ((m = form0.match(value)).hasMatch()) {
        // empty atoms
    }
    else if ((m = form1.match(value)).hasMatch())
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
        a = "-1";
        b = m.captured(2);
        c = m.captured(1);
    }
    else {
        // Forms with A / B are special, because B must be
        // fixed number. And the atom gets a hint.
        if ((m = form7.match(value)).hasMatch()) {
            a = m.captured(1);
            b = m.captured(2);
        }
        else if ((m = form8.match(value)).hasMatch()) {
            a = m.captured(1);
            b = m.captured(2);
            c = m.captured(3);
        }
        else if ((m = form9.match(value)).hasMatch()) {
            a = m.captured(1);
            b = m.captured(2);
            c = "-" + m.captured(3);
        }
        else {
            atomA = new AtomInvalid(value);
            return;
        }

        atomA = parseInputAtom(a);
        atomC = parseInputAtom(c);
        float bf = b.toFloat();
        if (bf == 0)
            atomB = new AtomInvalid(b);
        else
            atomB = new AtomNumber(1.0 / bf, ATOM_NUMBER_NUMBER, true);
        return;
    }
    atomA = parseInputAtom(a);
    atomB = parseInputAtom(b);
    atomC = parseInputAtom(c);
}

Atom *JackAssignmentInput::parseInputAtom(const QString &atom)
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

QList<PatchProblem *> JackAssignmentInput::collectProblems(const Patch *patch) const
{
    QList<PatchProblem *>problems;

    if (!atomA && !atomB && !atomC) {
        problems.append(
                    new PatchProblem(-1, 1, tr("You need to set a value for at least one of the three columns")));
    }
    // TODO: Mach endlich ne Liste!!![
    if (atomA) {
        QString text = atomA->problemAsInput(patch);
        if (text != "")
            problems.append(new PatchProblem(-1, 1, text));
    }
    if (atomB) {
        QString text = atomB->problemAsInput(patch);
        if (text != "")
            problems.append(new PatchProblem(-1, 2, text));
    }
    if (atomC) {
        QString text = atomC->problemAsInput(patch);
        if (text != "")
            problems.append(new PatchProblem(-1, 3, text));
    }
    return problems;
}

bool JackAssignmentInput::isUndefined() const
{
    return atomA == 0 && atomB == 0 && atomC == 0;
}

Atom *JackAssignmentInput::parseOnOff(QString s)
{
    if (s == "on")
        return new AtomNumber(1.0, ATOM_NUMBER_ONOFF, false);
    else if (s == "off")
        return new AtomNumber(0.0, ATOM_NUMBER_ONOFF, false);
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
        return new AtomNumber(number * factor, at, false);
}
