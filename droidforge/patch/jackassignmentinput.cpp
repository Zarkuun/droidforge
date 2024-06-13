#include "jackassignmentinput.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomnumber.h"
#include "globals.h"
#include "patch.h"

#include <QRegularExpression>
#include <QException>
#include <QCoreApplication>

#define NUM_ATOMS 3

JackAssignmentInput::JackAssignmentInput(QString jack, QString comment, QString valueString)
    : JackAssignment(jack, comment)
    , atoms{0, 0, 0}
{
    parseInputExpression(jack, valueString);
}

JackAssignmentInput::JackAssignmentInput(QString jack, QString comment)
    : JackAssignment(jack, comment)
    , atoms{0, 0, 0}
{
}

JackAssignmentInput::~JackAssignmentInput()
{
    for (int i=0; i<NUM_ATOMS; i++)
        if (atoms[i])
            delete atoms[i];
}
bool JackAssignmentInput::isSimple() const
{
    return atoms[0] && !atoms[1] && !atoms[2];
}
const Atom *JackAssignmentInput::atomAt(int column) const
{
    if (column > 0)
        return atoms[column-1];
    else
        return 0;
}
Atom *JackAssignmentInput::atomAt(int column)
{
    if (column > 0)
        return atoms[column-1];
    else
        return 0;
}
void JackAssignmentInput::replaceAtom(int column, Atom *newAtom)
{
    if (atoms[column-1])
        delete atoms[column-1];
    atoms[column-1] = newAtom;
}
void JackAssignmentInput::removeRegisterReferences(RegisterList &rl)
{
    for (int i=0; i<NUM_ATOMS; i++) {
        if (isInRegisterList(rl, atoms[i])) {
            delete atoms[i];
            atoms[i] = 0;
        }
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
    for (int i=0; i<NUM_ATOMS; i++)
        if (atoms[i])
            newas->atoms[i] = atoms[i]->clone();
    return newas;
}
void JackAssignmentInput::parseExpression(const QString &expression)
{
    parseInputExpression(jack, expression);
}
QString JackAssignmentInput::valueToString() const
{
    QString bOperator;
    if (atoms[1] && atoms[1]->isNumber() && ((AtomNumber *)atoms[1])->isFraction())
        bOperator = " / ";
    else
        bOperator = " * ";

    // Do more intelligent work for a nice transformation
    // into A * B + C
    if (!atoms[0] && !atoms[1] && !atoms[2]) // none defined
        return "";
    else if (atoms[0] && !atoms[1] && !atoms[2]) // just A
        return atoms[0]->toString();
    else if (!atoms[0] && atoms[1] && !atoms[2]) // just B
        return atoms[1]->toString();
    else if (!atoms[0] && !atoms[1] && atoms[2]) // just C
        return atoms[2]->toString();
    else if (atoms[0] && atoms[1] && !atoms[2]) // just A and B
        return atoms[0]->toString() + bOperator + atoms[1]->toString();
    else { // C is defined, but not just C
        QString sc, op;
        if (atoms[2]->isNegatable()) {
            sc = atoms[2]->toNegatedString();
            op = " - ";
        }
        else {
            sc = atoms[2]->toString();
            op = " + ";
        }
        if (atoms[0] && !atoms[1] && atoms[2]) // just A and C
            return atoms[0]->toString() + op + sc;
        else if (!atoms[0] && atoms[1] && atoms[2]) // just B and C
            return atoms[1]->toString() + op + sc;
        else
            return atoms[0]->toString() + bOperator + atoms[1]->toString() + op + sc;
    }
}

QString JackAssignmentInput::valueToCanonicalString() const
{
    if (!atoms[0] && !atoms[1] && !atoms[2]) // none defined
        return "";
    else if (atoms[0] && !atoms[1] && !atoms[2]) // just A
        return atoms[0]->toCanonicalString();
    else if (!atoms[0] && atoms[1] && !atoms[2]) // just B
        return atoms[1]->toCanonicalString();
    else if (!atoms[0] && !atoms[1] && atoms[2]) // just C
        return atoms[2]->toCanonicalString();
    else if (atoms[0] && atoms[1] && !atoms[2]) // just A and B
        return atoms[0]->toCanonicalString() + " * " + atoms[1]->toCanonicalString();
    else if (atoms[0] && !atoms[1] && atoms[2]) // just A and C
        return atoms[0]->toCanonicalString() + " + " + atoms[2]->toCanonicalString();
    else if (!atoms[0] && atoms[1] && atoms[2]) // just B and C
        return atoms[1]->toCanonicalString() + " + " + atoms[2]->toCanonicalString();
    else // A B and C
        return atoms[0]->toCanonicalString() + " * " + atoms[1]->toCanonicalString() + " + " + atoms[2]->toCanonicalString();
}
void JackAssignmentInput::collectCables(QStringList &cables) const
{
    for (int i=0; i<NUM_ATOMS; i++)
        if (atoms[i] && atoms[i]->isCable())
            cables.append(((AtomCable *)atoms[i])->getCable());
}
void JackAssignmentInput::findCableConnections(const QString &cable, int &asInput, int &) const
{
    for (int i=0; i<NUM_ATOMS; i++) {
        if (atoms[i] && atoms[i]->isCable() && ((AtomCable *)atoms[i])->getCable() == cable)
            asInput ++;
    }
}

#define RATOMA "[^*/+-]+"
#define RATOMB "-[0-9][^*/+-]*"
#define RATOM "(" RATOMA "|" RATOMB ")"
#define RNUMBER "(-?([0-9]+[.])?[0-9]+)"

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
            c = m.captured(4);
        }
        else if ((m = form9.match(value)).hasMatch()) {
            a = m.captured(1);
            b = m.captured(2);
            c = "-" + m.captured(4);
        }
        else {
            atoms[0] = new AtomInvalid(value);
            return;
        }

        atoms[0] = parseInputAtom(a);
        atoms[2] = parseInputAtom(c);
        float bf = b.toFloat();
        if (bf == 0)
            atoms[1] = new AtomInvalid(b);
        else
            atoms[1] = new AtomNumber(1.0 / bf, ATOM_NUMBER_FRACTION);
        return;
    }
    atoms[0] = parseInputAtom(a);
    atoms[1] = parseInputAtom(b);
    atoms[2] = parseInputAtom(c);
}
Atom *JackAssignmentInput::parseInputAtom(const QString &atom)
{
    if (atom == "")
        return 0;
    else if (atom[0] == '_')
        return parseCable(atom);
    else if (atom == "on" || atom == "off")
        return parseOnOff(atom);
    else if (atom[0].isDigit() || atom[0] == '-' || atom[0] == '.')
        return parseNumber(atom);
    else
        return parseRegister(atom);
}
Atom *JackAssignmentInput::parseInputFraction(const QString &s)
{
    QString withoutSpace = s;
    withoutSpace.replace(" ", "");
    if (withoutSpace.startsWith("1/")) {
        bool ok = false;
        double n = withoutSpace.mid(2).toDouble(&ok);
        if (n == 0)
            return 0;
        else if (ok)
            return  new AtomNumber(1.0 / n, ATOM_NUMBER_FRACTION);
        else
            return 0;
    }
    else
        return 0;
}
QList<PatchProblem *> JackAssignmentInput::collectSpecificProblems(const Patch *patch) const
{
    QList<PatchProblem *>problems;

    if (!atoms[0] && !atoms[1] && !atoms[2]) {
        problems.append(
                    new PatchProblem(-1, 1, TR("You need to set a value for at least one of the three columns")));
    }

    for (int i=0; i<NUM_ATOMS; i++) {
        if (atoms[i]) {
            QString text = atoms[i]->problemAsInput(patch);
            if (text != "")
                problems.append(new PatchProblem(-1, i+1, text));
        }
    }


    // Some hard coded cases, to better support the user
    if (atoms[0] && !atoms[1] && !atoms[2] && atoms[0]->isNumber()) {
        QString problemText;
        double number = ((AtomNumber *)atoms[0])->getNumber();
        if (jack == "encoder" || jack == "firstencoder" || jack == "fader" || jack == "firstfader" ) {
            if (number != round(number))
                problemText = TR("This value needs to be an integer number");
            else {
                int n = (int)number;
                if (n < 1)
                    problemText = TR("This value needs to be 1 or greater");
                else {
                    if (jack == "encoder" || jack == "firstencoder") {
                        unsigned numEncoders = patch->countEncoders();
                        if ((unsigned)n > numEncoders)
                            problemText = TR("Your setup has just %1 encoders, so you cannot use %2 here.").arg(numEncoders).arg(n);
                    }
                    else { // fader or firstfader
                        unsigned numFaders = patch->countFaders();
                        if ((unsigned)n > numFaders)
                            problemText = TR("Your setup has just %1 motorfaders, so you cannot use %2 here.").arg(numFaders).arg(n);
                    }
                }
            }
        }
        if (problemText != "") {
            problems.append(new PatchProblem(-1, 1, problemText));
        }
    }
    return problems;
}
bool JackAssignmentInput::isUndefined() const
{
    return atoms[0] == 0 && atoms[1] == 0 && atoms[2] == 0;
}
void JackAssignmentInput::incrementForExpansion(const Patch *patch)
{
    for (int a=0; a<=2; a++) {
        if (atoms[a])
            atoms[a]->incrementForExpansion(patch);
    }
}
bool JackAssignmentInput::sameAs(const JackAssignmentInput *other) const
{
    for (int a=0; a<=2; a++) {
        if (atoms[a] && !other->atoms[a])
            return false;
        else if (!atoms[a] && other->atoms[a])
            return false;
        else if (atoms[a] && other->atoms[a] && !atoms[a]->sameAs(other->atoms[a]))
            return false;
    }
    return true;
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
    double factor = 1.0;
    atom_number_t at = ATOM_NUMBER_NUMBER;
    if (s.endsWith("v") || s.endsWith("V")) {
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
    double number = s.toDouble(&ok);
    if (!ok)
        return 0;
    else
        return new AtomNumber(number * factor, at);
}
