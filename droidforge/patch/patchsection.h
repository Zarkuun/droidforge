#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"
#include "circuitchoosedialog.h"
#include "cursorposition.h"

#include <QList>

class PatchSection
{
    CursorPosition cursor;
    QString title;

public:
    QList<Circuit *> circuits;

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
    void addNewCircuit(int pos, QString name, jackselection_t jackSelection);
    void collectCables(QStringList &cables) const;

    const CursorPosition &cursorPosition() { return cursor; };
    void setCursor(const CursorPosition &pos) { cursor = pos; };
    Circuit *currentCircuit();
    Circuit *circuit(int n) { return circuits[n]; };
    JackAssignment *currentJackAssignment();
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
