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
    void setTitle(const QString &t) { title = t; };
    void deleteCurrentCircuit();
    void deleteCurrentJackAssignment();
    void deleteCurrentComment();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorLeft();
    void moveCursorRight();
    void setCursorRow(int row);
    void setCursorColumn(int column);
    void moveCursorToNextCircuit();
    void moveCursorToPreviousCircuit();
    void addNewCircuit(int pos, QString name, jackselection_t jackSelection);
    void collectCables(QStringList &cables) const;

    const CursorPosition &cursorPosition() { return cursor; };
    void setCursor(const CursorPosition &pos) { cursor = pos; };
    Circuit *currentCircuit();
    JackAssignment *currentJackAssignment();
    bool needG8() const;
    bool needX7() const;

protected:
};

#endif // PATCHSECTION_H
