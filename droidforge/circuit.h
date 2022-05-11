#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "jackassignment.h"

#include <QList>
#include <QString>
#include <QStringList>

class Circuit
{
    QString name;
    QStringList comment;
    QList<JackAssignment *> jackAssignments;

public:
    Circuit(QString name, const QStringList &comment);
    ~Circuit();
    Circuit *clone() const;
    QString toString();
    QString getName() const { return name; };
    QString getComment() const;
    void setComment(QString c);
    bool hasComment() const { return !comment.empty(); };
    void removeComment();
    qsizetype numCommentLines() const { return comment.size(); };
    void addJackAssignment(JackAssignment *);
    void insertJackAssignment(JackAssignment *, int index);
    JackAssignment *jackAssignment(unsigned i) { return jackAssignments[i]; };
    qsizetype numJackAssignments() const { return jackAssignments.count(); };
    void deleteJackAssignment(unsigned i);
    const Atom *atomAt(int row, int column) const;
    void collectCables(QStringList &cables) const;

};

#endif // CIRCUIT_H
