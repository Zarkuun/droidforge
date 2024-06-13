#ifndef JACKDEDUPLICATOR_H
#define JACKDEDUPLICATOR_H

#include "jackassignment.h"

#include <QMap>
#include <QString>

class JackDeduplicator
{
    bool deduplicate;
    unsigned jacktableOffset;
    unsigned savedBytes;
    QString circuitName;
    QMap<QString, unsigned> sharedValues;

public:
    JackDeduplicator(bool dedup);
    QString processJackAssignment(const JackAssignment *ja);
    void setCircuit(QString name) { circuitName = name; };
    unsigned jacktableSize() const { return jacktableOffset; };
    unsigned saved() const { return savedBytes; };
};

#endif // JACKDEDUPLICATOR_H
