#include "patchgeneratorbase.h"
#include "globals.h"
#include "patchgenerator.h"

PatchGeneratorBase *the_patch_generator_base = 0;

PatchGeneratorBase::PatchGeneratorBase(QDir directory)
    : _directory(directory)
{
    the_patch_generator_base = this;
    loadGenerators();
}
void PatchGeneratorBase::loadGenerators()
{
    for (auto &file: _directory.entryList()) {
        if (file.startsWith('.')) {
            continue;
        }
        PatchGenerator *gen = new PatchGenerator(_directory.absoluteFilePath(file));
        if (gen->isValid())
            _generators.append(gen);
        else {
            shout << "Patch generator" << file << "failed:" << gen->error();
            delete gen;
        }
    }
}
