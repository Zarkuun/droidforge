#include "jackassignment.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomregister.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "parseexception.h"
#include "atomregister.h"

#include <QRegularExpression>

JackAssignment::JackAssignment(QString jack, QString comment)
    : jack(jack)
    , comment(comment)
    , disabled(false)
{
}
JackAssignment::~JackAssignment()
{
}
QString JackAssignment::toString() const
{
    QString s = disabled ? "#   " : "    ";
    s += jack + " =";
    QString v = valueToString();
    if (v != "")
        s += " " + v;
    if (!comment.isEmpty())
        s += " # " + comment;
    return s;
}
QString JackAssignment::toBare() const
{
    if (disabled)
        return "";
    else
        return jack + "=" + valueToString().replace(" ", "");
}
QString JackAssignment::jackPrefix() const
{
    QString prefix = jack;
    while (prefix != "" && prefix.back().isDigit())
        prefix.chop(1);
    return prefix;
}
JackAssignment *JackAssignment::parseJackLine(const QString &circuit, QString line)
{
    QStringList parts = line.split("#");
    if (parts[0].count('=') != 1)
        throw ParseException("Duplicate =");

    QString comment;
    if (parts.size() > 1)
        comment = parts.mid(1).join('#').trimmed();

    parts = parts[0].split("=");
    QString jack = parts[0].trimmed().toLower();
    QString valueString = parts[1].trimmed();

    JackAssignment *ja;
    jack = the_firmware->canonizeJackName(circuit, jack); // e.g. pitch -> pitch1
    if (the_firmware->jackIsInput(circuit, jack))
        ja = new JackAssignmentInput(jack, comment, valueString);
    else if (the_firmware->jackIsOutput(circuit, jack))
        ja = new JackAssignmentOutput(jack, comment, valueString);
    else
        ja = new JackAssignmentUnknown(jack, comment, valueString);
    return ja;

}
bool JackAssignment::needG8() const
{
    for (int i=1; i<=3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->needG8())
             return true;
    }
    return false;
}
bool JackAssignment::needX7() const
{
    for (int i=1; i<=3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->needX7())
             return true;
    }
    return false;
}
void JackAssignment::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (int i=1; i<=3; i++) {
         Atom *atom = atomAt(i);
         if (atom)
             atom->swapControllerNumbers(fromNumber, toNumber);
    }
}
void JackAssignment::shiftControllerNumbers(int number, int by)
{
    for (int i=1; i<=3; i++) {
         Atom *atom = atomAt(i);
         if (atom)
             atom->shiftControllerNumbers(number, by);
    }
}
void JackAssignment::collectRegisterAtoms(RegisterList &sl) const
{
    for (int i=1; i<=3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->isRegister()) {
             AtomRegister *ar = (AtomRegister *)atom;
             sl.append(*ar);
         }
    }
}
void JackAssignment::remapRegister(AtomRegister from, AtomRegister to)
{
    for (int i=1; i<=3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->isRegister()) {
             AtomRegister *ar = (AtomRegister *)atom;
             if (*ar == from)
                 replaceAtom(i, to.clone());
         }
    }
}
Atom *JackAssignment::parseCable(QString s)
{
    static QRegularExpression exp("^_[a-z0-9_]+$",  QRegularExpression::CaseInsensitiveOption);

    if (exp.match(s).hasMatch())
        return new AtomCable(s.mid(1).toUpper());
    else
        return 0;

}
Atom *JackAssignment::parseRegister(QString s)
{
    // Note: we allow invalid registers such as I0 here. It's easer
    // for creating precise error messages later.
    static QRegularExpression expa("^([INOGRX])([0-9]+)$", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression expb("^([BLPS])([0-9]+)[.]([0-9]+)$", QRegularExpression::CaseInsensitiveOption);

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

    else
        return new AtomInvalid(s);
}
bool numericStringLess(const QString &a, const QString &b)
{
    if (b == "")
        return false;
    else if (a == "")
        return true;
    else if (a[0] == b[0])
        return numericStringLess(a.mid(1), b.mid(1));

    if (a[0].isDigit() && !b[0].isDigit())
        return false;
    else if (!a[0].isDigit() && b[0].isDigit())
        return true;
    else if (!a[0].isDigit() && !b[0].isDigit())
        return numericStringLess(a.mid(1), b.mid(1));

    // OK. Now we have two digits. We do not compare the digits
    // but the number that is constructed by all following digits

    auto na = a.toULongLong();
    auto nb = b.toULongLong();
    if (na < nb)
        return true;
    else if (nb < na)
        return false;

    // Identical numbers. Skip.
    int i = 0;
    while (a[i].isDigit() && i<a.size())
        i++;
    return numericStringLess(a.mid(i), b.mid(i));
}
bool operator<(const JackAssignment &a, const JackAssignment &b)
{
    if (a.jackType() < b.jackType())
        return true;
    else if (a.jackType() > b.jackType())
        return false;

    return numericStringLess(a.jack, b.jack); // TODO: button2 < button16
}
