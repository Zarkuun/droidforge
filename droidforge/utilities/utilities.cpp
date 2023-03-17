#include "utilities.h"
#include "globals.h"

#include <QRegularExpression>


QString saveFilename(QString name)
{
    static QRegularExpression re("  *");
    for (char c: BANNED_FILENAME_CHARS) {
        shout << "Weg mit " << c;
        name.remove(QChar(c));
    }
    name.replace(re, " ");
    return name;
}
