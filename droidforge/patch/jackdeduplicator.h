#ifndef JACKDEDUPLICATOR_H
#define JACKDEDUPLICATOR_H

#include "jackassignment.h"

#include <QMap>
#include <QString>

class JackDeduplicator
{
    bool deduplicate;
    bool useShortnames;
    unsigned jacktableOffset;
    unsigned savedBytes;
    QString circuitName;
    QMap<QString, unsigned> sharedValues;

public:
    JackDeduplicator(bool dedup, bool shorts);
    bool useShorts() const { return useShortnames; };
    QString processJackAssignment(const JackAssignment *ja);
    void setCircuit(QString name) { circuitName = name; };
    QString circuit() const { return circuitName; };
    unsigned jacktableSize() const { return jacktableOffset; };
    unsigned saved() const { return savedBytes; };
};

#endif // JACKDEDUPLICATOR_H
