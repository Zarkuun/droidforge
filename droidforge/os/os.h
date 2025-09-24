#ifndef OS_H
#define OS_H

#include <QtGlobal>

#ifdef Q_OS_MAC
    #define ZERO_WIDTH_SPACE QString::fromUtf8("\u200C")
#endif
#ifdef Q_OS_WIN
    #define ZERO_WIDTH_SPACE QString("")
#endif
#ifdef Q_OS_LINUX
    #define ZERO_WIDTH_SPACE QString("")
#endif
#ifndef ZERO_WIDTH_SPACE
    #error "ZERO_WIDTH_SPACE not defined!"
#endif

#endif // OS_H
