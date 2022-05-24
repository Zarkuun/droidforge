#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "circuit.h"
#include "jackassignment.h"
#include "atom.h"
#include "patchsection.h"
#include "selection.h"

#include <QList>

class Clipboard
{
    QList<Circuit *> circuits;
    QList<JackAssignment *> jackAssignments;
    QList<Atom *> atoms;
    QString comment;

public:
    Clipboard();
    ~Clipboard();
    void copyFromSelection(Selection *sel, PatchSection *section);
    bool isEmpty() const;
    bool isCircuits() const;
    const QList<Circuit *> *getCircuits() const { return &circuits; };

private:
    void clear();
};

#endif // CLIPBOARD_H
