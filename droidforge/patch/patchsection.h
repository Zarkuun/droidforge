#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"
#include "circuitchoosedialog.h"
#include "cursorposition.h"
#include "patchproblem.h"

#include <QList>

class Patch;

class PatchSection
{
    CursorPosition cursor;
    QString title;

public:
    QList<Circuit *> circuits; // TODO: Make this private

    PatchSection() {}; // no title
    PatchSection(QString t) : title(t) {};
    ~PatchSection();
    PatchSection *clone() const;
    QString toString() const;
    QString getTitle() const { return title; };
    QString getNonemptyTitle() const;
    void setTitle(const QString &t) { title = t; };
    void deleteCurrentCircuit();
    void deleteCircuit(int i);
    void deleteCurrentJackAssignment();
    void deleteCurrentComment();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorLeft();
    void moveCursorRight();
    void setCursorRow(int row);
    void setCursorColumn(int column);
    void sanitizeCursor();
    void moveCursorToNextCircuit();
    void moveCursorToPreviousCircuit();
    void addNewCircuit(QString name, jackselection_t jackSelection);
    void addCircuit(int pos, Circuit *circuit);
    void addCircuit(Circuit *circuit);

    void collectCables(QStringList &cables) const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    QList<PatchProblem *> collectProblems(const Patch *patch) const;

    const CursorPosition &cursorPosition() const { return cursor; };
    void setCursor(const CursorPosition &pos) { cursor = pos; };
    Circuit *currentCircuit();
    const Circuit *currentCircuit() const;
    const Atom *atomAt(const CursorPosition &pos);
    void setAtomAt(const CursorPosition &pos, Atom *atom);
    bool isEmpty() const { return circuits.size() == 0; };
    Circuit *circuit(int n) { return circuits[n]; };
    const Circuit *circuit(int n) const { return circuits[n]; };
    const QList<Circuit *> &getCircuits() const { return circuits; };
    JackAssignment *currentJackAssignment();
    const JackAssignment *currentJackAssignment() const;
    bool needG8() const;
    bool needX7() const;
    void swapControllerNumbers(int fromNumber, int toNumber);
    void shiftControllerNumbers(int number, int by);
    void collectRegisterAtoms(RegisterList &) const;
    void remapRegister(AtomRegister from, AtomRegister to);
    void removeRegisterReferences(RegisterList &rl, int ih, int oh);

protected:
};

#endif // PATCHSECTION_H
