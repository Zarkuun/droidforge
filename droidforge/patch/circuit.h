#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"
#include "patchproblem.h"

#include <QList>
#include <QString>
#include <QStringList>

typedef enum {
    JACKSELECTION_FULL = 0,
    JACKSELECTION_TYPICAL = 1,
    JACKSELECTION_ESSENTIAL = 2,
    JACKSELECTION_NONE = 3,
} jackselection_t;


class Patch;

class Circuit
{
    QString name;
    QStringList comment;
    QList<JackAssignment *> jackAssignments;
    bool disabled;
    bool folded;
    bool haveBookmark;
    int bmRow;
    int bmColumn;

public:
    Circuit(QString name, const QStringList &comment, bool disabled);
    ~Circuit();
    Circuit *clone() const;
    QString toString() const;
    QString toCleanString() const;
    QString toBare() const;
    QString getName() const { return name; };
    QString getComment() const;
    QString getCommentWithHashes() const;
    bool isDisabled() const { return disabled; };
    bool isFolded() const { return folded; };
    void toggleFold() { folded = !folded; };
    void setFold(bool f) { folded = f; };
    void setDisabledWithJacks(bool d);
    void setEnabled() { disabled = false; };
    void setComment(QString c);
    bool hasComment() const { return !comment.empty(); };
    void removeComment();
    qsizetype numCommentLines() const { return comment.size(); };
    void addJackAssignment(JackAssignment *);
    void insertJackAssignment(JackAssignment *, int index);
    JackAssignment *jackAssignment(unsigned i) { return jackAssignments[i]; };
    const JackAssignment *jackAssignment(unsigned i) const { return jackAssignments[i]; };
    const JackAssignment *findJack(const QString name) const;
    bool hasUndefinedJacks() const;
    void removeUndefinedJacks();
    QStringList missingJacks(jacktype_t jackType) const;
    bool hasMissingJacks() const;
    bool hasLEDMismatch();
    void fixLEDMismatches();
    qsizetype numJackAssignments() const { return jackAssignments.count(); };
    QString nextJackArrayName(const QString &jackName, bool isInput);
    void deleteJackAssignment(unsigned i);
    void sortJacks();
    void sortJacksFromTo(int fromRow, int toRow);
    const Atom *atomAt(int row, int column) const;
    Atom *atomAt(int row, int column);
    void setAtomAt(int row, int column, Atom *atom);
    void collectCables(QStringList &cables) const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    void rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode, int fromRow=0, int toRow=-1);
    QList<PatchProblem *> collectProblems(const Patch *patch) const;
    bool jackIsArray(int row) const;
    QString prefixOfJack(const QString &jackName);
    unsigned memoryFootprint() const;
    bool needsX7() const;
    bool usesSelect() const;
    bool hasBookmark() const { return haveBookmark; };
    void setBookmark(int row, int column);
    void clearBookmark() { haveBookmark = false; };
    int bookmarkRow() const { return bmRow; };
    int bookmarkColumn() const { return bmColumn; };

    void changeCircuit(QString newCircuit);
    void collectRegisterAtoms(RegisterList &, bool skipOverlayedControls=false) const;
    void removeRegisterReferences(RegisterList &rl);

private:
    bool checkLEDMismatches(bool fixit);
    JackAssignment *findJack(const QString name);
};

#endif // CIRCUIT_H
