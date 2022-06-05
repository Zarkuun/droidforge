#ifndef JACKASSIGNMENT_H
#define JACKASSIGNMENT_H

#include "atom.h"
#include "atomregister.h"
#include "patchproblem.h"
#include "registerlist.h"

#include <QString>
#include <QSet>

class Patch;
class Circuit;

typedef enum {
    JACKTYPE_INPUT = 0,
    JACKTYPE_OUTPUT = 1,
    JACKTYPE_UNKNOWN = 2,
    JACKTYPE_DONT_CARE = 3,
} jacktype_t;


class JackAssignment
{
protected:
    QString jack;
    QString comment;
    bool disabled;

public:
    JackAssignment(QString jack, QString comment = "");
    void setDisabled(bool d) { disabled = d; };
    bool isDisabled() const { return disabled; };
    QString toString() const;
    QString jackName() const { return jack; };
    void setJackName(const QString &name) { jack = name; };
    QString getComment() const { return comment; };
    void setComment(const QString &c) { comment = c; };
    void changeJack(QString j) { jack = j; };
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromNumber, int toNumber);
    void shiftControllerNumbers(int number, int by);
    void collectRegisterAtoms(RegisterList &) const;
    void remapRegister(AtomRegister from, AtomRegister to);

    virtual ~JackAssignment();
    virtual JackAssignment *clone() const = 0;
    virtual jacktype_t jackType() const = 0;
    virtual QString valueToString() const = 0;
    virtual bool isInput() const { return false; };
    virtual bool isOutput() const { return false; };
    virtual const Atom *atomAt(int column) const = 0;
    virtual Atom *atomAt(int column) = 0;
    virtual void replaceAtom(int column, Atom *newAtom) = 0;
    virtual void collectCables(QStringList &cables) const = 0;
    virtual void findCableConnections(const QString &, int &, int &) const {};
    virtual void parseExpression(const QString &expression) = 0;
    virtual void removeRegisterReferences(RegisterList &rl, int ih, int oh) = 0;
    virtual QList<PatchProblem *> collectProblems(const Patch *patch) const = 0;
    virtual bool isUndefined() const = 0;
    static JackAssignment *parseJackLine(const QString &circuit, QString line);

protected:
    static Atom *parseCable(QString s);
    static Atom *parseRegister(QString s);

     friend bool operator<(const JackAssignment &a, const JackAssignment &b);
};

bool operator<(const JackAssignment &a, const JackAssignment &b);

#endif // JACKASSIGNMENT_H
