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


void JackAssignment::parseInputValue()
{
    garbled = true;
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
        return new AtomCable(s.toUpper());
    else
        return 0;

}


Atom *JackAssignment::parseRegister(QString s)
{
    static QRegularExpression expa("^([IORX])([1-9][0-9])*$", QRegularExpression::CaseInsensitiveOption);
    static QRegularExpression expb("^([BLPS])([1-9][0-9]*)\\.([1-9][0-9])$", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch m;

    m = expa.match(s);
    if (m.hasMatch()) {
        QChar registerType = m.captured(0).toUpper()[0];
        unsigned number = m.captured(1).toUInt();
        return new AtomRegister(registerType, 0, number);
    }

    m = expb.match(s);
    if (m.hasMatch()) {
        QChar registerType = m.captured(0).toUpper()[0];
        unsigned controller = m.captured(1).toUInt();
        unsigned number = m.captured(2).toUInt();
        return new AtomRegister(registerType, controller, number);
    }

    return 0;
}
