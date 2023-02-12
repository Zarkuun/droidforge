#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include "circuit.h"
#include "jackassignment.h"
#include "atom.h"
#include "patch.h"
#include "patchsection.h"
#include "selection.h"

#include <QList>

class Clipboard : public QObject
{
    Q_OBJECT

    QList<Circuit *> circuits;
    QList<JackAssignment *> jackAssignments;
    QList<Atom *> atoms;
    QString comment;
    qint64 lockGlobalClipboardUntil;

public:
    Clipboard();
    Clipboard(const QList<Circuit *>);
    ~Clipboard();
    void copyFromSelection(const Selection *sel, const PatchSection *section);
    bool isEmpty() const;
    unsigned numCircuits() const;
    unsigned numJacks() const;
    unsigned numAtoms() const;
    bool isComment() const;
    const QList<Circuit *> &getCircuits() const { return circuits; };
    const QString &getComment() const { return comment; };
    const QList<JackAssignment *> getJackAssignment() const { return jackAssignments; };
    const QList<Atom *> &getAtoms() const { return atoms; };
    Patch *getAsPatch() const;
    void copyToGlobalClipboard() const;
    void copyFromGlobalClipboard();
    QString toString() const;

private:
    void clear();

signals:
    void changed();
};

extern Clipboard *the_clipboard;

#endif // CLIPBOARD_H
