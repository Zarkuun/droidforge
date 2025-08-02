#include "atomtext.h"
#include "globals.h"

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
