#include "atomtext.h"
#include "globals.h"
#include "tuning.h"

AtomText *AtomText::clone() const
{
    return new AtomText(text);
}

QString AtomText::toString() const
{
    return QString("\"") + text + "\"";
}

QString AtomText::problemAsInput(const Patch *) const
{
    if (text.length() > DB8E_MAX_TEXT_LENGTH)
        return TR("The maximum allowed text length is %1, yours is %2")
            .arg(DB8E_MAX_TEXT_LENGTH)
            .arg(text.length());
    else
        return "";
}

QString AtomText::problemAsOutput(const Patch *) const
{
    return TR("You cannot use a text as an output parameter");
}

QString AtomText::cleanText(QString text)
{
    QString cleanedText = "";
    QChar c;
    for (unsigned i=0; i<text.length(); i++) {
        c = text[i];
        int ord = c.unicode();
        if (ord >= 32 && ord <= 126 and ord != 34 /* quote */)
            cleanedText += c;
    }
    return cleanedText;
}
