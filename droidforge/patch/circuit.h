#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"
#include "patchproblem.h"

#include <QList>
#include <QString>
#include <QStringList>

class Patch;

class Circuit
{
    QString name;
    QStringList comment;
    QList<JackAssignment *> jackAssignments;
    bool disabled;

public:
    Circuit(QString name, const QStringList &comment, bool disabled);
    ~Circuit();
    Circuit *clone() const;
    QString toString();
    QString getName() const { return name; };
    QString getComment() const;
    bool isDisabled() const { return disabled; };
    void setDisabledWithJacks(bool d);
    void setComment(QString c);
    bool hasComment() const { return !comment.empty(); };
    void removeComment();
    qsizetype numCommentLines() const { return comment.size(); };
    void addJackAssignment(JackAssignment *);
    void insertJackAssignment(JackAssignment *, int index);
    JackAssignment *jackAssignment(unsigned i) { return jackAssignments[i]; };
    const JackAssignment *jackAssignment(unsigned i) const { return jackAssignments[i]; };
    qsizetype numJackAssignments() const { return jackAssignments.count(); };
    void deleteJackAssignment(unsigned i);
    const Atom *atomAt(int row, int column) const;
    Atom *atomAt(int row, int column);
    void setAtomAt(int row, int column, Atom *atom);
    void collectCables(QStringList &cables) const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    QList<PatchProblem *> collectProblems(const Patch *patch) const;

    void changeCircuit(QString newCircuit);
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromNumber, int toNumber);
    void shiftControllerNumbers(int number, int by);
    void collectRegisterAtoms(RegisterList &) const;
    void remapRegister(AtomRegister from, AtomRegister to);
    void removeRegisterReferences(RegisterList &rl, int ih, int oh);
};

#endif // CIRCUIT_H
