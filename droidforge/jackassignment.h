#ifndef JACKASSIGNMENT_H
#define JACKASSIGNMENT_H

#include "atom.h"

#include <QString>
#include <QSet>

typedef enum {
    JACKTYPE_INPUT = 0,
    JACKTYPE_OUTPUT = 1,
    JACKTYPE_UNKNOWN = 2,
    JACKTYPE_DONT_CARE = 3,
} jacktype_t;


class JackAssignment
{

protected:
    QString jack;
    QString comment;
    bool disabled;

public:
    JackAssignment(QString jack, QString comment = "");
    virtual ~JackAssignment();
    virtual JackAssignment *clone() const = 0;
    virtual jacktype_t jackType() const = 0;
    QString toString() const;
    virtual QString valueToString() const = 0;
    QString jackName() const { return jack; };
    virtual const Atom *atomAt(int column) const = 0;
    virtual void replaceAtom(int column, Atom *newAtom) = 0;
    virtual void collectCables(QStringList &cables) const = 0;
    void changeJack(QString j) { jack = j; };
    static JackAssignment *parseJackLine(const QString &circuit, QString line);
    virtual void parseExpression(const QString &expression) = 0;

protected:
    static Atom *parseCable(QString s);
    static Atom *parseRegister(QString s);
};

#endif // JACKASSIGNMENT_H
