#ifndef JACKASSIGNMENT_H
#define JACKASSIGNMENT_H

#include "atom.h"
#include "atomregister.h"
#include "patchproblem.h"
#include "registerlist.h"
#include "rewritecablesdialog.h"

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
    QString toBare() const;
    QString jackName() const { return jack; };
    QString jackPrefix() const;
    void setJackName(const QString &name) { jack = name; };
    QString getComment() const { return comment; };
    void setComment(const QString &c) { comment = c; };
    void changeJack(QString j) { jack = j; };
    void collectRegisterAtoms(RegisterList &, bool skipControls=false) const;
    void rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode);
    void incrementForExpansion(const Patch *patch);
    QList<PatchProblem *> collectProblems(const Patch *patch) const;

    virtual ~JackAssignment();
    virtual JackAssignment *clone() const = 0;
    virtual jacktype_t jackType() const = 0;
    virtual QString valueToString() const = 0;
    virtual bool isInput() const { return false; };
    virtual bool isOutput() const { return false; };
    virtual int numColumns() const { return 1; };
    virtual const Atom *atomAt(int column) const = 0;
    virtual Atom *atomAt(int column) = 0;
    virtual void replaceAtom(int column, Atom *newAtom) = 0;
    virtual void collectCables(QStringList &cables) const = 0;
    virtual void findCableConnections(const QString &, int &, int &) const {};
    virtual void parseExpression(const QString &expression) = 0;
    virtual void removeRegisterReferences(RegisterList &rl) = 0;
    virtual QList<PatchProblem *> collectSpecificProblems(const Patch *patch) const = 0;
    virtual bool isUndefined() const = 0;
    static JackAssignment *parseJackLine(const QString &circuit, QString line);

protected:
    static Atom *parseCable(QString s);
    static Atom *parseRegister(QString s);
    friend bool operator<(const JackAssignment &a, const JackAssignment &b);
};

bool operator<(const JackAssignment &a, const JackAssignment &b);

#endif // JACKASSIGNMENT_H
