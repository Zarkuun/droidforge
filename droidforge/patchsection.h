#ifndef PATCHSECTION_H
#define PATCHSECTION_H

#include "circuit.h"
#include "cursorposition.h"

#include <QList>

class PatchSection
{
    CursorPosition cursor;

public:
    QString title;
    QList<Circuit *> circuits;

    PatchSection(QString t) : title(t) {};
    ~PatchSection();
    PatchSection *clone() const;
    QString toString();
    void deleteCurrentCircuit();
    void deleteCurrentJackAssignment();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorToNextCircuit();
    void moveCursorToPreviousCircuit();
    const CursorPosition &cursorPosition() { return cursor; };
    void setCursor(const CursorPosition &pos) { cursor = pos; };
    Circuit *currentCircuit();
    JackAssignment *currentJackAssignment();

protected:
};

#endif // PATCHSECTION_H
