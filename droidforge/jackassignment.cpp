#include "jackassignment.h"
#include "atomcable.h"
#include "atominvalid.h"
#include "atomregister.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "jackassignmentunknown.h"
#include "parseexception.h"

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
    QString s;
    if (disabled)
        s += "# ";
    s = "    " + jack + " = " + valueToString();
    if (!comment.isEmpty())
        s += " # " + comment;
    return s;
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
    for (int i=0; i<3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->needG8())
             return true;
    }
    return false;
}

bool JackAssignment::needX7() const
{
    for (int i=0; i<3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->needX7())
             return true;
    }
    return false;
}

void JackAssignment::swapControllerNumbers(int fromindex, int toindex)
{
    for (int i=0; i<3; i++) {
         Atom *atom = atomAt(i);
         if (atom)
             atom->swapControllerNumbers(fromindex, toindex);
    }
}

void JackAssignment::collectRegisterAtoms(QStringList &sl) const
{
    for (int i=0; i<3; i++) {
         const Atom *atom = atomAt(i);
         if (atom)
             atom->collectRegisterAtoms(sl);
    }
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
    static QRegularExpression expa("^([INOGRX])([1-9][0-9]*)$", QRegularExpression::CaseInsensitiveOption);
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

    else
        return new AtomInvalid(s);
}
