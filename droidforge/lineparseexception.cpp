#include "lineparseexception.h"


QString LineParseException::toString() const
{
    return QString("Line ") + QString::number(lineNo) + ": " + reason;
}
