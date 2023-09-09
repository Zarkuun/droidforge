#ifndef OS_H
#define OS_H

#include <QtGlobal>

#ifdef Q_OS_MAC
    #define ZERO_WIDTH_SPACE QString::fromUtf8("\u200C")
#else // Windows
    #define ZERO_WIDTH_SPACE QString("")
#endif

#endif // OS_H
