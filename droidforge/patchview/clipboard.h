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
    bool isComment() const;
    bool isJacks() const;
    bool isAtoms() const;
    const QList<Circuit *> &getCircuits() const { return circuits; };
    const QString &getComment() const { return comment; };
    const QList<JackAssignment *> getJackAssignment() const { return jackAssignments; };
    const QList<Atom *> &getAtoms() const { return atoms; };

private:
    void clear();
};

#endif // CLIPBOARD_H
