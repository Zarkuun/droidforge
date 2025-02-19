#ifndef PATCHGENERATORBASE_H
#define PATCHGENERATORBASE_H

#include <QDir>
#include <QList>

class PatchGenerator;

class PatchGeneratorBase
{
    QDir _directory;
    QList<PatchGenerator *> _generators;

public:
    PatchGeneratorBase(QDir directory);
    QList<PatchGenerator *> *generators() { return &_generators; };
    bool enableGenerators();
    void disableGenerators();
    bool informAndAsk() const;

private:
    bool deployGenerators();
    bool loadGenerators();
};

extern PatchGeneratorBase *the_patch_generator_base;

#endif // PATCHGENERATORBASE_H
