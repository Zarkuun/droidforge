#include "utilities.h"
#include "tuning.h"

#include <QRegularExpression>


QString saveFilename(QString name)
{
    static QRegularExpression re("  *");
    for (char c: BANNED_FILENAME_CHARS) {
        name.remove(QChar(c));
    }
    name.replace(re, " ");
    return name;
}

QString niceBytes(unsigned int bytes)
{
    QString s = QString::number(bytes);
    if (bytes > 9999) {
        QString end = s.mid(s.length() - 3);
        QString start = s.mid(0, s.length() - 3);
        return start + "," + end;
    }
    else
        return s;

}
