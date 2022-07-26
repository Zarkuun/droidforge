#ifndef OS_H
#define OS_H

#include <QtGlobal>

#ifdef Q_OS_MAC
    #define ZERO_WIDTH_SPACE QString::fromUtf8("\u200C")
    #define HAVE_PDF 1
    #define HAVE_MIDI 1
#else // Windows
    #define ZERO_WIDTH_SPACE QString("")
    #define HAVE_PDF 1
#endif

#endif // OS_H
