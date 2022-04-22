#ifndef GENERALPARSEEXCEPTION_H
#define GENERALPARSEEXCEPTION_H

#include <QException>

class GeneralParseException : public QException
{
    QString reason;

public:
    GeneralParseException(QString reason) : reason(reason) {};
    QString toString() const { return reason; };
};

#endif // GENERALPARSEEXCEPTION_H
