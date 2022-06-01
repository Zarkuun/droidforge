#ifndef GLOBALS_H
#define GLOBALS_H

#include <QCoreApplication>

// Variant of tr for contexts without an QObject
#define TR(s) QCoreApplication::translate("Patch", s)

#endif // GLOBALS_H
