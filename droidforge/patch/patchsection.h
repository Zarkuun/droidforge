#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"
#include "cursorposition.h"
#include "jackassignmentinput.h"
#include "patchproblem.h"
#include "selection.h"
#include "rewritecablesdialog.h"
#include "jackdeduplicator.h"

#include <QList>

class Patch;

class PatchSection
{
    QStringList comment;
    CursorPosition cursor;
    QString title;
    Selection *selection;
    QList<Circuit *> circuits;

public:
    PatchSection(); // no title
    PatchSection(QString t);
    ~PatchSection();
    PatchSection *clone() const;
    QString toString(bool suppressEmptyHeader) const;
    QString toBareString() const;
    QString toDeployString(JackDeduplicator &jdd) const;
    QString getTitle() const { return title; };
    QString getNonemptyTitle() const;
    void setComment(const QStringList &c);
    const QStringList &getComment() const { return comment; };
    void setTitle(const QString &t) { title = t; };
    void deleteCurrentCircuit();
    void deleteCircuit(int i);
    void deleteCurrentJackAssignment();
    void deleteCurrentComment();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorForward();
    void moveCursorBackward();
    void setCursorRow(int row);
    void setCursorRowColumn(int row, int column);
    void setCursorColumn(int column);
    void sanitizeCursor();
    CursorPosition canonizedCursorPosition(const CursorPosition &pos) const;
    void moveCursorToNextCircuit();
    void moveCursorToPreviousCircuit();
    void addNewCircuit(QString name, jackselection_t jackSelection, int position=-1);
    void insertCircuit(int pos, Circuit *circuit);
    void addCircuit(Circuit *circuit);
    void duplicateCurrentCircuit();
    bool allCircuitsFolded() const;
    void toggleFold();
    void rewriteSelectedCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode);

    const Selection *getSelection() const { return selection; };
    const Selection * const *getSelectionPointer() const { return &selection; };
    Patch *getSelectionAsPatch() const;
    void clearSelection();
    void selectAll();
    void setMouseSelection(const CursorPosition &to);
    void updateKeyboardSelection(const CursorPosition &before, const CursorPosition &after);

    void collectCables(QStringList &cables) const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    QList<PatchProblem *> collectProblems(const Patch *patch) const;

    const CursorPosition &cursorPosition() const { return cursor; };
    void setCursor(const CursorPosition &pos);
    void setCursorNoUnfold(const CursorPosition &pos);
    Circuit *currentCircuit();
    const Circuit *currentCircuit() const;
    int currentCircuitId() const { return cursor.circuitNr; };
    const Atom *currentAtom() const;
    const Atom *atomAt(const CursorPosition &pos) const;
    Atom *atomAt(const CursorPosition &pos);
    void setAtomAt(const CursorPosition &pos, Atom *atom);
    bool isEmpty() const { return circuits.size() == 0; };
    Circuit *circuit(int n) { return circuits[n]; };
    const Circuit *lastCircuit() const { return circuits.last(); };
    unsigned numCircuits() const { return circuits.count(); };
    const Circuit *circuit(int n) const { return circuits[n]; };
    const QList<Circuit *> &getCircuits() const { return circuits; };
    void swapCircuits(int ida, int idb);
    JackAssignment *currentJackAssignment();
    const JackAssignment *currentJackAssignment() const;
    JackAssignment *jackAssignmentAt(const CursorPosition &pos);
    void collectRegisterAtoms(RegisterList &, bool skipOverlayedControls) const;
    void removeRegisterReferences(RegisterList &rl);
    unsigned ramUsedByCircuits() const;
    bool searchHitAtCursor(const QString &text);
    bool needsMIDI() const;
    void clearBookmarks();
    bool findBookmark(CursorPosition *pos);
    void setBookmark();

private:
    bool nextCursorPosition(CursorPosition &pos) const;
    bool nextCircuitCursorPosition(CursorPosition &pos) const;
    bool previousCursorPosition(CursorPosition &pos) const;
};



#endif // PATCHSECTION_H
