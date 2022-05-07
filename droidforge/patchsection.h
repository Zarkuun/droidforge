#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"
#include "circuitchoosedialog.h"
#include "cursorposition.h"

#include <QList>

class PatchSection
{
    CursorPosition cursor;

public:
    QString title;
    QList<Circuit *> circuits;

    PatchSection() {}; // no title
    PatchSection(QString t) : title(t) {};
    ~PatchSection();
    PatchSection *clone() const;
    QString toString();
    void deleteCurrentCircuit();
    void deleteCurrentJackAssignment();
    void deleteCurrentComment();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorLeft();
    void moveCursorRight();
    void setCursorRow(int row);
    void moveCursorToNextCircuit();
    void moveCursorToPreviousCircuit();
    void addNewCircuit(int pos, QString name, jackselection_t jackSelection);

    const CursorPosition &cursorPosition() { return cursor; };
    void setCursor(const CursorPosition &pos) { cursor = pos; };
    Circuit *currentCircuit();
    JackAssignment *currentJackAssignment();

protected:
};

#endif // PATCHSECTION_H
