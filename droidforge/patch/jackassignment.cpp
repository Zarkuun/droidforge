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
#include "tuning.h"
#include "globals.h"
#include "jackdeduplicator.h"

#include <QRegularExpression>
#include <QSettings>

JackAssignment::JackAssignment(QString jack, QString comment)
    : jack(jack)
    , comment(comment)
    , disabled(false)
{
}
JackAssignment::~JackAssignment()
{
}
QString JackAssignment::toDeployString(JackDeduplicator &jdd) const
{
    if (disabled)
        return "";

    QString name;
    if (jdd.useShorts())
        name = the_firmware->jackShortname(jdd.circuit(), jack);
    if (name == "")
        name = jack;
    return name + "=" + jdd.processJackAssignment(this);
}
QString JackAssignment::toString() const
{
    QString s;
    if (disabled)
        s = "#   ";
    else
        s = "    ";

    s += jack + " =";
    QString v = valueToString(false /* compressed */);
    if (v != "")
        s += " " + v;

    if (!comment.isEmpty())
        s += " # " + comment;
    return s;
}
QString JackAssignment::toBareString() const
{
    if (disabled)
        return "";
    else
        return jack + " = " + valueToString(false /* compressed */);
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
    int commentMark = findCommentMark(line);
    QString comment;
    QString noncomment;
    if (commentMark != -1) {
        noncomment = line.left(commentMark).trimmed();
        comment = line.mid(commentMark + 1).trimmed();
    }
    else {
        noncomment = line.trimmed();
        comment = "";
    }

    int equal_pos = noncomment.indexOf('=');
    if (equal_pos == -1)
        throw ParseException("Missing =");

    QString jack = noncomment.left(equal_pos).toLower().trimmed();
    QString valueString = noncomment.mid(equal_pos + 1).trimmed();

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
void JackAssignment::collectRegisterAtoms(RegisterList &sl, bool skipControls) const
{
    for (int i=1; i<=3; i++) {
         const Atom *atom = atomAt(i);
         if (atom && atom->isRegister()) {
             AtomRegister *ar = (AtomRegister *)atom;
             if (skipControls && ar->isControl())
                 continue;
             if (!sl.contains(*ar))
                 sl.append(*ar);
         }
    }
}
void JackAssignment::rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode)
{
    for (int i=1; i<=numColumns(); i++) {
         Atom *atom = atomAt(i);
         if (atom)
             atom->rewriteCableNames(remove, insert, mode);
    }
}
void JackAssignment::rewriteCablePrefixes(const QString &fromPrefix, const QString &toPrefix)
{
    for (int i=1; i<=3; i++) {
         Atom *atom = atomAt(i);
         if (atom)
             atom->rewriteCablePrefix(fromPrefix, toPrefix);
    }
}
void JackAssignment::renameCable(const QString &oldName, const QString &newName)
{
    for (int i=1; i<=3; i++) {
        Atom *atom = atomAt(i);
        if (atom && atom->isCable())
        {
            AtomCable *ac = (AtomCable *)atom;
            if (ac->getCable() == oldName)
                ac->setCable(newName);
        }
    }
}
QList<PatchProblem *> JackAssignment::collectProblems(const Patch *patch) const
{
    QList<PatchProblem *>problems = collectSpecificProblems(patch);

    // check line length if cable name compression is off
    QSettings settings;
    bool renameCables = settings.value("compression/rename_cables", false).toBool();
    if (!renameCables) {
        QString  line = toBareString();
        if (line.length() > MAX_PATCH_LINE_LENGTH)
            problems.append(new PatchProblem(-1, 0,
              TR("Line too long: "
                 "The maximum line length is %1, "
                 "this line is %2. Use shorter cable names or "
                 "enable compression of cable names.")
                                             .arg(MAX_PATCH_LINE_LENGTH)
                                             .arg(line.length())));
    }
    return problems;
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
int JackAssignment::findCommentMark(QString text)
{
    // Examples:
    // "foo = 12"
    // "foo = 12 # This is 12"
    // "foo = 12 # This is 12 and this is a hash: #"
    // "header = \"A hash #\"
    // "header = \"A hash #\" # comment

    bool inQuotes = false;
    int hashPos = -1;

    for (int i = 0; i < text.size(); ++i) {
        QChar c = text.at(i);

        if (c == '"') {
            inQuotes = !inQuotes; // Quote-Zustand umschalten
        }
        else if (c == '#' && !inQuotes) {
            hashPos = i;
            break;
        }
    }
    return hashPos;
}
unsigned long prefixNumber(const QString &s)
{
    // "13in6" -> 13
    int i=0;
    while (s[i].isDigit())
        i++;
    QString n = s.left(i);
    return n.toULong();
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

    auto na = prefixNumber(a);
    auto nb = prefixNumber(b);
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
