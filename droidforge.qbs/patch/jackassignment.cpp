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
    AtomRegister ar(s);
    if (ar.isNull())
        return new AtomInvalid(s);
    else
        return ar.clone();
}
bool numericStringLess(const QString &a, const QString &b)
{
    if (b == "")
        return false;
    else if (a == "")
        return true;
    else if (a[0] == b[0])
        return numericStringLess(a.mid(1), b.mid(1));
    else if (a[0].isDigit() && !b[0].isDigit())
        return false;
    else if (!a[0].isDigit() && b[0].isDigit())
        return true;
    else if (!a[0].isDigit() && !b[0].isDigit())
        return a[0] < b[0];

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

    return numericStringLess(a.jack, b.jack);
}