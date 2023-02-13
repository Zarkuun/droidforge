#include <QFile>

#include "parseexception.h"
#include "statusdump.h"
#include "globals.h"

/*
DROID status

Firmware version: blue-2
Running since:    0.835 sec
Free RAM:         71963 Bytes (63.684%)
Size of patch:    10437 Bytes (16.307%)

Inputs:
    I1:  0.0027    I2:  0.0037    I3:  0.0792    I4:  0.0789
    I5:  0.0000    I6:  0.0000    I7:  0.0000    I8:  0.0000

Normalizations:
    N1:  1.0000    N2:  0.0000    N3:  0.0000    N4:  0.0500
    N5:  0.0000    N6:  0.0000    N7:  0.0000    N8:  0.0000

Outputs:
    O1:  0.0000    O2:  0.0000    O3:  0.0000    O4:  0.0000
    O5:  0.0000    O6:  0.0000    O7:  0.0000    O8:  0.0000

Gates:
    G1:       0    G2:       0    G3:       0    G4:       0
    G5:       0    G6:       0    G7:       0    G8:       0
    G9:       0    G10:      0    G11:      0    G12:      0

RGB-LEDs:
    R1:   0.000    R2:   0.000    R3:   0.000    R4:   0.000
    R5:   0.000    R6:   0.000    R7:   0.000    R8:   0.000
    R9:   0.000    R10:  0.000    R11:  0.000    R12:  0.000
    R13:  0.000    R14:  0.000    R15:  0.000    R16:  0.000
    R17:  0.000    R18:  0.000    R19:  0.000    R20:  0.000
    R21:  0.000    R22:  0.000    R23:  0.000    R24:  0.000
    R25:  0.000    R26:  0.000    R27:  0.000    R28:  0.000
    R29:  0.000    R30:  0.000    R31:  0.000    R32:  0.000

Controller 1 [p4b2]:
    B1.1:       0    B1.2:       0
    L1.1:   0.000    L1.2:   1.000
    P1.1:  1.0000    P1.2:  0.0000    P1.3:  0.0000    P1.4:  0.6249
    P1.5:  0.0002    P1.6:  0.2499    P1.7:  0.0000    P1.8:  0.0000

Controller 2 [s10]:

Controller 3 [p2b8]:
    B3.1:       0    B3.2:       0    B3.3:       0    B3.4:       0
    B3.5:       0    B3.6:       0    B3.7:       0    B3.8:       0
    L3.1:   0.000    L3.2:   0.000    L3.3:   0.000    L3.4:   0.000
    L3.5:   0.000    L3.6:   0.000    L3.7:   0.000    L3.8:   0.000
    P3.1:  0.0000    P3.2:  0.4132

Internal patch cables:
    _MIDI_USB_CLOCK:  0.00000
    _MIDI_USB_IN_RESET: 0.00000
    _MIDI_USB_CLOCK_PRESENT: 0.00000
    _MIDI_DIN_CLOCK:  0.00000
    _MIDI_DIN_IN_RESET: 0.00000
    _MIDI_DIN_CLOCK_PRESENT: 0.00000
    _INTERNAL_CLOCK:  1.00000
    _RUNNING:         1.00000
    _RESET:           0.00000
    _CLOCK:           0.00000
*/

StatusDump::StatusDump(const QString &filePath)
{
    QStringList lines;
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        throw ParseException("Cannot open file: " + inputFile.errorString());
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    inputFile.close();
    parse(lines);
}
bool StatusDump::hasCable(const QString &name) const
{
    return cables.contains(name);
}
bool StatusDump::hasRegister(const QString &name) const
{
    return registers.contains(name);

}
double StatusDump::valueOfCable(const QString &name) const
{
    return cables[name];
}
double StatusDump::valueOfRegister(const QString &name) const
{
    return registers[name];
}
void StatusDump::parse(const QStringList &lines)
{
    for (auto line: lines) {
        if (!line.startsWith(" "))
            continue; // data lines begin with spaces
        line = line.trimmed();
        if (line.startsWith("_"))
            parseCableLine(line);
        else
            parseRegisterLine(line);
    }
}
void StatusDump::parseCableLine(const QString &line)
{
    // "_INTERNAL_CLOCK:  1.00000"
    auto parts = line.split(':');
    QString name = parts[0].mid(1);
    double value = parts[1].trimmed().toDouble();
    cables[name] = value;

}
void StatusDump::parseRegisterLine(const QString &line)
{
    // "P1.5:  0.0002    P1.6:  0.2499    P1.7:  0.0000    P1.8:  0.0000"
    auto parts = line.split(' ', Qt::SkipEmptyParts);
    for (int i=0; i<parts.count(); i+=2) {
        QString reg = parts[i].chopped(1);
        double value = parts[i+1].toDouble();
        registers[reg] = value;
    }
}
