#ifndef GLOBALS_H
#define GLOBALS_H

#include <QCoreApplication>
#include <QDebug>

// Variant of tr for contexts without an QObject
#define TR(s) QCoreApplication::translate("Patch", s)
#define shout qDebug()
#define shoutfunc qDebug() << Q_FUNC_INFO

#endif // GLOBALS_H
