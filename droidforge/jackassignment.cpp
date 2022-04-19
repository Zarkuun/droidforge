#include "jackassignment.h"
#include "atomcable.h"
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
    QString s;
    if (disabled)
        s += "# ";
    s = "    " + jack + " = " + valueToString();
    if (!comment.isEmpty())
        s += " # " + comment;
    return s;
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
