#ifndef LINEPARSEEXCEPTION_H
#define LINEPARSEEXCEPTION_H

#include <QException>

class LineParseException : public QException
{
    unsigned lineNo;
    QString reason;

public:
    LineParseException(unsigned lineNo, const QString &reason)
        : lineNo(lineNo), reason(reason) {};
    QString toString() const;
};

#endif // LINEPARSEEXCEPTION_H
