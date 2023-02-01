#ifndef PARSEEXCEPTION_H
#define PARSEEXCEPTION_H

#include <QException>

class ParseException : public QException
{
    QString reason;

public:
    ParseException(QString reason) : reason(reason) {};
    QString toString() const { return reason; };
};

#endif // PARSEEXCEPTION_H
