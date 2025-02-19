#include "droidfirmware.h"
#include "globals.h"
#include "registertypes.h"

#include <QFile>
#include <QJsonArray>
#include <QRegularExpression>
#include <QMessageBox>

#define GROUP "([^}]*)"
QRegularExpression replace_("_{" GROUP "}");
QRegularExpression replaceT("{\\\\t " GROUP "}");
QRegularExpression replaceNth("\\\\nth ?([4-9])");
QRegularExpression replaceNthX("\\\\nth{" GROUP "}");
QRegularExpression replaceIt("{\\\\it" GROUP "}");
QRegularExpression replaceBf("{\\\\bf" GROUP "}");
QRegularExpression replaceSqrt("\\\\sqrt{" GROUP "}");
QRegularExpression replaceFootnotesize("{\\\\footnotesize" GROUP "}");
QRegularExpression replaceCircuit("\\\\circuit{" GROUP "}");
QRegularExpression replaceFrac("\\\\frac{" GROUP "}{" GROUP "}");
QRegularExpression replaceFramebox("\\\\framebox[^{]*{" GROUP "}");
QRegularExpression regTabular("\\\\(begin|end){tabular}.*");
QRegularExpression regVspace("\\\\vspace{[^}]*}");
QRegularExpression regJacktable("\\\\jacktablerow{" GROUP "}{" GROUP "}");
QRegularExpression regJacktableE("\\\\jacktablerowE{" GROUP "}{" GROUP "}{" GROUP "}");
QRegularExpression regSup("\\^([2-9])");
DroidFirmware *the_firmware = 0;

DroidFirmware::DroidFirmware()
{
    the_firmware = this;

    QFile file;
    file.setFileName(":droidfirmware.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open firmware information file.");
    }
    else {
        QByteArray content = file.readAll();
        json = QJsonDocument::fromJson(content);
        file.close();
        circuits = json["circuits"].toObject();
        controllers = json["controllers"].toObject();
        pagerefs = json["manual_references"].toObject();
    }

    // Activate the following line for checking all circuit jack descriptions
    // for LaTeX artefacts that should not be there
    // checkAllDescriptions();
}
QString DroidFirmware::version() const
{
    return json["firmware_version"].toString();
}
unsigned DroidFirmware::availableMemory(unsigned master) const
{
    QString key = "master" + QString::number(master);
    return json["available_memory"].toObject()[key].toInt();
}
unsigned int DroidFirmware::initialJacktableSize() const
{
    return json["jacktable_initial_size"].toInt();
}
unsigned DroidFirmware::manualPage(QString pageref) const
{
    return pagerefs[pageref].toInt(0);
}
unsigned DroidFirmware::hasManualPage(QString pageref) const
{
    return pagerefs.contains(pageref);
}
bool DroidFirmware::circuitExists(QString circuit) const
{
    return circuits.keys().contains(circuit);
}
unsigned DroidFirmware::circuitManualPage(QString circuit) const
{
    return circuits[circuit].toObject()["manual"].toInt();
}
unsigned DroidFirmware::circuitBaseRAM(QString circuit) const
{
    return circuits[circuit].toObject()["ramsize"].toInt();
}
bool DroidFirmware::circuitIsPersisted(QString circuit) const
{
    // All circuits that have presets (und thus a preset input) are persisted.
    return jackIsInput(circuit, "preset");
}
bool DroidFirmware::circuitIsDeprecated(QString circuit) const
{
    auto object = circuits[circuit].toObject();
    QString cat = object["category"].toString();
    return cat == "deprecated";
}
QString DroidFirmware::jackRamHint(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    if (!jackinfo.isNull())
        return jackinfo["ramhint"].toString();
    jackinfo = findJack(circuit, "outputs", jack);
    if (!jackinfo.isNull())
        return jackinfo["ramhint"].toString();
    return "undefined"; // Should never be reached.

}
unsigned DroidFirmware::controllerUsedRAM(QString controller) const
{
    return controllers[controller].toObject()["ramsize"].toInt();
}
QString DroidFirmware::canonizeJackName(QString circuit, QString jack) const
{
    // "buttongroup", "button" -> "button1"
    // "copy", "i" -> "input"
    // "mixer", "i2" -> "input2"
    // "matrixmixer", "b13" -> "button13"

    // We need to first check all singular jacks, than the arrays.
    // This is in order not to mix up [algoquencer] pitch, which must
    // not be converted to "pitch1", since it means the output "pitch".

    for (int check_array = 0; check_array < 2; check_array ++)
    {
        QString whence = "inputs";
        while (true) {
            // Loop through all jacks of the current type (inputs or outputs)
            // twice. First for singular jacks, than for arrays (because of
            // the algoquencer pitch problem.
            QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
            for (qsizetype i=0; i<jacklist.size(); i++) {
                QJsonObject jackinfo = jacklist[i].toObject();

                int is_array = jackinfo.contains("count") ? 1 : 0;
                if (check_array != is_array)
                    continue;

                QString shortname = jackinfo["short"].toString("");

                if (is_array) {
                    // First try full name. Example "buttonoutput1"
                    // Beware: don't parse "buttonoutput1" into "button1" for [buttongroup]
                    QString prefix = jackinfo["prefix"].toString(); // e.g "button"
                    int prefixlen = prefix.length(); // 6
                    QString front = jack.mid(0, prefixlen); //  "button"
                    if (front == prefix && front.length() < jack.length() && isDigits(jack.mid(prefixlen)))
                        return jack;
                    else if (jack == prefix)
                        return prefix + "1"; // "buttongroup", "button" -> "button1"

                    // Now try short name
                    int shortlen = shortname.length();
                    front = jack.mid(0, shortlen);
                    if (front == shortname && front.length() < jack.length() && isDigits(jack.mid(shortlen)))
                        return prefix + jack.mid(shortlen); // replace short by long prefix
                    else if (jack == shortname)
                        return prefix + "1"; // "buttongroup", "b" -> "button1"
                }

                else { // single parameter
                    QString name = jackinfo["name"].toString("");
                    if (jack == shortname || jack == name)
                        return name;
                }
            }

            if (whence == "outputs")
                break;
            else
                whence = "outputs";
        }
    }
    return jack; // unknown
}
bool DroidFirmware::jackIsInput(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    return !jackinfo.isNull();
}
bool DroidFirmware::jackIsOutput(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "outputs", jack);
    return !jackinfo.isNull();
}
unsigned DroidFirmware::jackArraySize(QString circuit, QString prefix, bool isInput) const
{
    QJsonValue jackinfo = findJackArray(circuit, isInput ? "inputs" : "outputs", prefix);
    if (jackinfo.isNull())
        return 0;

    QJsonObject ji = jackinfo.toObject();
    if (ji.contains("count"))
        return jackinfo["count"].toInt(1);
    else
        return 0;
}

QString DroidFirmware::jackShortname(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    if (jackinfo.isNull())
        jackinfo = findJack(circuit, "outputs", jack);
    if (jackinfo.isNull())
        return ""; // unknown jack

    QString prefix = jackinfo["prefix"].toString("");
    QString suffix;
    if (prefix != "")
        suffix = jack.mid(prefix.length());

    QString shortname = jackinfo["short"].toString("");
    if (shortname == "")
        return "";

    return shortname + suffix;
}
QString DroidFirmware::jackTypeSymbol(QString circuit, QString prefix, bool isInput) const
{
    const char *whence = isInput ? "inputs" : "outputs";
    QJsonValue jackinfo = findJack(circuit, whence, prefix);
    if (jackinfo.isNull())
        jackinfo = findJackArray(circuit, whence, prefix);
    if (jackinfo.isNull())
        return "";

    QJsonObject ji = jackinfo.toObject();
    return ji["type"].toString();
}
QStringList DroidFirmware::circuitsOfCategory(QString category) const
{
    QStringList result;
    for (auto i=circuits.constBegin(); i != circuits.constEnd(); ++i)
    {
        QString name = i.key();
        QString cat = i.value().toObject()["category"].toString();
        if (cat == "deprecated")
            continue; // Omit deprecated circuits in search
        if (category.isEmpty() || cat == category)
            result.append(name);
    }
    return result;
}
QString DroidFirmware::circuitDescription(QString circuit) const
{
    auto object = circuits[circuit].toObject();
    QString latexcode = object["description"].toString();
    QString firstSentence = latexcode.split('.')[0].replace("\n", " ") + ".";
    QString fullDescription = delatexify(firstSentence);
    return fullDescription;
}
QString DroidFirmware::circuitTitle(QString circuit) const
{
    return delatexify(circuits[circuit].toObject()["title"].toString());
}
QStringList DroidFirmware::inputsOfCircuit(QString circuit, jackselection_t jackSelection) const
{
    return jacksOfCircuit(circuit, "inputs", jackSelection);
}
QStringList DroidFirmware::outputsOfCircuit(QString circuit, jackselection_t jackSelection) const
{
    return jacksOfCircuit(circuit, "outputs", jackSelection);
}
QStringList DroidFirmware::jacksOfCircuit(QString circuit, QString whence, jackselection_t jackSelection) const
{
    QStringList result;
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();

    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        int essential = jackinfo["essential"].toInt(0);
        // HACK: I don't know how I can avoid this easily, but the motor fader
        // circuits usually depend on "select" being used.
        if ((circuit == "faderbank" || circuit == "motorfader")) {
            if (jackinfo["name"] == "select")
                essential = 2;
            else if (jackinfo["name"] == "selectat")
                essential = 1;
        }

        // essential is 0, 1 or 2 (0 = none, 1 = typical, 2 = essential)
        // jackSelection is 0 -> all, ... 3 -> none
        if (essential >= jackSelection) {
            int count;
            if (jackSelection != JACKSELECTION_FULL && jackinfo.contains("essential_count"))
                count = jackinfo["essential_count"].toInt(1);
            else if (jackinfo.contains("count"))
                count = jackinfo["count"].toInt(1);
            else
                count = 0;

            if (count > 0) {
                for (int i=1; i<=count; i++) {
                    QString name = jackinfo["prefix"].toString() + QString::number(i);
                    result.append(name);
                }
            }
            else
                result.append(jackinfo["name"].toString());
        }
    }
    return result;
}
QStringList DroidFirmware::jackGroupsOfCircuit(QString circuit, QString whence, QString search) const
{
    QStringList result;
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        QString name;
        if (jackinfo.contains("count"))
            name = jackinfo["prefix"].toString();
        else
            name = jackinfo["name"].toString();
        if (name.contains(search, Qt::CaseInsensitive))
            result.append(name);
    }
    return result;
}
QString DroidFirmware::jackDescriptionHTML(QString circuit, QString whence, QString jack) const
{
    QString description;

    QJsonValue jackinfo = findJack(circuit, whence, jack);
    if (!jackinfo.isNull()) {
        QJsonObject info = jackinfo.toObject();
        QString desc = info["description"].toString();
        int i = desc.indexOf("\\begin{tabular}");
        if (i >= 0)
            desc = desc.mid(0, i);
        description = delatexify(desc, true /* html */);
    }
    else
        description = TR("Sorry, this parameter is not documented, yet.");

    auto table = jackValueTable(circuit, whence, jack);
    if (!table.empty())
        description += jackTableAsString(table);
    else
        description += "<br>";

    if (whence == "inputs" && the_firmware->jackHasDefaultvalue(circuit, jack)) {
        double default_value = the_firmware->jackDefaultvalue(circuit, jack);
        description += "<br>" + TR("Default value: %1").arg(default_value) + "<br>";
    }
    return description;
}
QString DroidFirmware::jackTypeDescriptionHTML(QString circuit, QString whence, QString jack) const
{
    QString symbol = jackTypeSymbol(circuit, jack, whence == "inputs");
    return jackTypeExplanation(symbol, whence == "inputs");
}
QString DroidFirmware::jackTypeSymbol(QString circuit, QString whence, QString jack) const
{
    return jackTypeSymbol(circuit, jack, whence == "inputs");
}
QString DroidFirmware::jackTableAsString(const QMap<double, QString> &table) const
{
    QString text = "<br><br>";
    for (auto it = table.keyBegin(); it != table.keyEnd(); ++it)
    {
        double value = *it;
        QString description = table[value];
        text += QString::number(value) + ": " + description + "<br>";
    }
    return text;
}
QString DroidFirmware::jackTypeExplanation(QString symbol, bool isInput) const
{
    if (isInput) {
        if (symbol == "cv")
            return TR("This input accepts any types of CV, may it be continous or stepped.");
        else if (symbol == "integer")
            return TR("This input expects integer numbers like 0, 1, 2 and so on. Other values are rounded to the nearest integer number.");
        else if (symbol == "stepped")
            return TR("This input expects stepped CVs.");
        else if (symbol == "voltperoctave")
            return TR("This input interpretes CV on a 1 Volt per octave basis.");
        else if (symbol == "trigger")
            return TR("This input looks just for positive trigger edges, i.e. when the value goes from below 1V to 1V or above. It does not take into account the length of the gate.");
        else if (symbol == "gate")
            return TR("This input just distinguishes between the state \"low\", which is everyting below 1V and \"high\" which is at 1V or above.");
        else if (symbol == "bipolar")
            return TR("This input expects a value between 0.0 and 1.0. The value 0.5 is special and means the neutral or center position. Negative input values are converted to 0.0. Values about 1.0 are regarded as 1.0.");
        else if (symbol == "fraction")
            return TR("This input expects a value between 0.0 and 1.0 (or 0% and 100%). Negative input values are converted to 0.0. Values about 1.0 are regarded as 1.0");

    }
    else {
        if (symbol == "cv")
            return TR("This output outputs arbitrary types of CV values.");
        else if (symbol == "integer")
            return TR("This output ouputs integer number likes 0, 1, 2 and so on.");
        else if (symbol == "stepped")
            return TR("This output outputs stepped CV values.");
        else if (symbol == "voltperoctave")
            return TR("The CV values of this output adher to a precise 1 Volt per octave scheme. You can use them to control the pitch of oscillators.");
        else if (symbol == "trigger")
            return TR("This output sends short triggers with a length of 10 ms and a value of 1.0 (10 V).");
        else if (symbol == "gate")
            return TR("This output sends either 0.0 or 1.0 (10 V).");
        else if (symbol == "bipolar")
            return TR("This output sends a value between 0.0 and 1.0");
        else if (symbol == "fraction")
            return TR("This output sends a value between 0.0 and 1.0");
    }

    return "ARGL:" + symbol;
}
bool DroidFirmware::isDigits(QString s) const
{
    for (int i=0; i<s.length(); i++) {
        if (!s[i].isDigit())
            return false;
    }
    return true;
}
QMap<double, QString> DroidFirmware::jackValueTable(QString circuit, QString whence, QString jack) const
{
    QMap<double, QString> table;
    QJsonValue jackinfo = findJack(circuit, whence, jack);
    if (!jackinfo.isNull()) {
        QString desc = jackinfo.toObject()["description"].toString();
        QStringList parts = desc.split('\n');
        for (auto &line: parts) {
            line.replace(replaceT, "<tt>\\1</tt>");
            auto match = regJacktable.match(line);
            if (!match.hasMatch())
                match =  regJacktableE.match(line);
            if (!match.hasMatch())
                continue;

            QString value = match.captured(1);
            QString description = delatexify(match.captured(2));
            table[value.toDouble()] = description;
        }
    }
    return table;
}
bool DroidFirmware::jackHasDefaultvalue(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    if (!jackinfo.isNull()) {
        if (jackinfo.toObject()["default"].isNull())
            return false;
        QString s = jackinfo.toObject()["default"].toString();
        return s[0].isDigit();
    }
    else
        return false;
}
double DroidFirmware::jackDefaultvalue(QString circuit, QString jack) const
{
    QJsonValue jackinfo = findJack(circuit, "inputs", jack);
    QString s = jackinfo.toObject()["default"].toString();
    return s.toDouble();
}
unsigned DroidFirmware::numGlobalRegisters(char registerType) const
{
    switch (registerType) {
    case REGISTER_INPUT:     return 8;
    case REGISTER_NORMALIZE: return 8;
    case REGISTER_OUTPUT:    return 8;
    case REGISTER_GATE:      return 12;
    case REGISTER_RGB_LED:   return 56;
    case REGISTER_EXTRA:     return 1;
    default:                 return 0;
    }

}
bool DroidFirmware::circuitNeedsMaster18(QString circuit) const
{
    return circuit == "vcotuner"
            || circuit == "sinfonionlink";
}
bool DroidFirmware::circuitNeedsMIDI(QString circuit) const
{
    return     circuit == "midiin"
            || circuit == "midiout"
            || circuit == "midithrough"
            || circuit == "firefacecontrol";
}
bool DroidFirmware::checkAllDescriptions() const
{
    for (auto& circuitname: circuits.keys()) {
        for (auto whence: { "inputs", "output" }) {
            QJsonArray inputs = circuits[circuitname].toObject()[whence].toArray();
            for (auto input: inputs) {
                QJsonObject jackinfo = input.toObject();
                QString jack = jackinfo["name"].toString();
                QString desc = jackDescriptionHTML(circuitname, whence, jack);
                if (desc.contains("\\") || desc.contains("}") || desc.contains("}"))
                {
                    if (QMessageBox::warning(
                        0,
                        "LaTeX reminents in jack description",
                        QString("Circuit: ") + circuitname + "\n"
                             "Jack: " + jack + "\n\n"
                             "Description: \n" +
                             desc,
                              QMessageBox::Ok | QMessageBox::Abort) == QMessageBox::Abort)
                    return false;
                }
            }
        }
    }
    return true;
}
unsigned DroidFirmware::numControllerRegisters(const QString &module, char registerType) const
{
    if (module == "p4b2") {
        switch (registerType) {
        case REGISTER_POT:    return 4;
        case REGISTER_BUTTON: return 2;
        case REGISTER_LED:    return 2;
        }
    }
    else if (module == "p2b8") {
        switch (registerType) {
        case REGISTER_POT:    return 2;
        case REGISTER_BUTTON: return 8;
        case REGISTER_LED:    return 8;
        }
    }
    else if (module == "p10") {
        switch (registerType) {
        case REGISTER_POT:    return 10;
        }
    }
    else if (module == "b32") {
        switch (registerType) {
        case REGISTER_BUTTON: return 32;
        case REGISTER_LED:    return 32;
        }
    }
    else if (module == "s10") {
        switch (registerType) {
        case REGISTER_SWITCH: return 10;
        }
    }
    if (module == "p8s8") {
        switch (registerType) {
        case REGISTER_POT:    return 8;
        case REGISTER_LED:    return 8;
        case REGISTER_SWITCH: return 8;
        }
    }
    else if (module == "e4") {
        switch (registerType) {
        case REGISTER_ENCODER: return 4;
        case REGISTER_BUTTON:  return 4;
        case REGISTER_LED:     return 4;
        }
    }
    else if (module == "m4") {
        switch (registerType) {
        case REGISTER_POT:     return 4;
        case REGISTER_BUTTON:  return 4;
        case REGISTER_RGB_LED: return 4;
        case REGISTER_LED:     return 4;
        }
    }

    return 0;
}
QJsonValue DroidFirmware::findJack(QString circuit, QString whence, QString jack) const
{
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        // Account for jack arrays
        if (jackinfo.contains("count")) {
            for (qsizetype i=1; i<=jackinfo["count"].toInt(); i++) {
                QString n = jackinfo["prefix"].toString() + QString::number(i);
                if (n == jack)
                    return jackinfo;
            }
            continue;
        }
        else if (jackinfo["name"] == jack)
            return jackinfo;
    }
    return QJsonValue(QJsonValue::Null);
}
QJsonValue DroidFirmware::findJackArray(QString circuit, QString whence, QString prefix) const
{
    QJsonArray jacklist = circuits[circuit].toObject()[whence].toArray();
    for (qsizetype i=0; i<jacklist.size(); i++) {
        QJsonObject jackinfo = jacklist[i].toObject();
        if (jackinfo["prefix"].toString() == prefix)
            return jackinfo;
    }
    return 0;
}
QString DroidFirmware::delatexify(QString s, bool html) const
{
    s.replace(regTabular, "");
    s.replace(regVspace, "");
    s.replace("\n\n", "<PARAGRAPH>");
    s.replace("\n", " ");
    s.replace("<PARAGRAPH>", "\n\n");
    s.replace("$", "");
    s.replace("<", html ? "&lt;" : "<");
    s.replace(">", html ? "&gt;" : ">");

    s.replace("\\msixteen", "MASTER");
    s.replace("\\meighteen", "MASTER18");
    s.replace("\\nth1", html ? "1<sup>st</sup>" : "first");
    s.replace("\\nth2", html ? "2<sup>nd</sup>" : "second");
    s.replace("\\nth3", html ? "3<sup>rd</sup>" : "third");
    s.replace("\\th", html ? "<sup>th</sup>" : "th");
    s.replace(replaceNth, html ? "\\1<sup>th</sup>" : "\\1th");
    s.replace(replaceNthX, html ? "\\1<sup>th</sup>" : "\\1th");
    s.replace(regSup, html ? "<sup>\\1</sup>" : "\\1");

    replaceLatexSymbols(s);

    s.replace("\\customcolortable", "0.2 = cyan, 0.4 = green, 0.6 = yellow, 0.73 = orange, 0.8 = red, 1.0 = magenta, 1.1 = violet, 1.2 = blue");
    s.replace("\\normalsize", "");
    s.replace("\\medskip", "\n\n");
    s.replace("\\droid", "DROID");
    s.replace("$\\times$", " X ");
    s.replace("\\begin{itemize}", html ? "<ul>" : "");
    s.replace("\\end{itemize}", html ? "</ul>" : "");
    s.replace("\\item", html ? "<br><li>" : "");
    if (s.startsWith("\\textcolor{red}{\\bf "))
        s = s.mid(20);
    s.replace(replaceFramebox, "\\1");
    s.replace(replaceT,       html ? "<tt>\\1</tt>" : "\"\\1\"");
    s.replace(replaceIt,      html ? "<i>\\1</i>"   : "\\1");
    s.replace(replaceBf,      html ? "<b>\\1</b>"   : "\\1");
    s.replace(replaceCircuit, html ? "<tt>\\1</tt>" : "\"\\1\"");
    s.replace(replace_, html ? "<sub>\\1</sub>" : "\\1");
    s.replace(replaceSqrt, "√\\1");
    s.replace("\\sqrt", "√");
    s.replace("\\infty", "∞");
    s.replace(replaceFrac, "\\1 / \\2");
    s.replace(replaceFootnotesize, "\\1");

    // cleanup some crap that our poor bogus parser did not catch
    s.replace("{\\t", "");
    s.replace("\\nth{13", "13th");

    return s;
}
void DroidFirmware::replaceLatexSymbols(QString &s) const
{
    s.replace("^\\circ", "°");
    s.replace("\\%", "%");
    s.replace("\\times", " X ");
    s.replace("\\ ", " ");
    s.replace("--", "-");
    s.replace("\\ding{192}", "➀");
    s.replace("\\ding{193}", "➁");
    s.replace("\\ding{194}", "➂");
    s.replace("\\ding{195}", "➃");
    s.replace("\\ding{196}", "➄");
    s.replace("\\ding{197}", "➅");
    s.replace("\\ding{202}", "➊");
    s.replace("\\ding{203}", "➋");
    s.replace("\\ding{204}", "➌");
    s.replace("\\ding{205}", "➍");
    s.replace("\\ding{206}", "➎");
    s.replace("\\ding{207}", "➏");
    s.replace("\\rightarrow", "→");
    s.replace("\\leftarrow",  "←");
    s.replace("\\&" , "&");
    s.replace("~", " ");
    s.replace("''", "\"");
    s.replace("``", "\"");
    s.replace("\\dots", "...");
    s.replace("\\pi", "𝜋");
    s.replace("\\sharp", "♯");
    s.replace("\\flat", "♭");
    s.replace("\\#", "#");
    s.replace("\\infty", "∞");
}
