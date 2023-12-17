#include "patchgeneratorbase.h"
#include "globals.h"
#include "patchgenerator.h"

#include <QMessageBox>
#include <QDirIterator>

PatchGeneratorBase *the_patch_generator_base = 0;

PatchGeneratorBase::PatchGeneratorBase(QDir directory)
    : _directory(directory)
{
    the_patch_generator_base = this;
    deployGenerators();
    loadGenerators();
}
void PatchGeneratorBase::deployGenerators()
{
    QDirIterator it(":pg");
    while (it.hasNext()) {
        QString ressourcePath = it.next();
        auto parts = ressourcePath.split("/");
        QString fileName = parts[1];
        QString absPath = _directory.absoluteFilePath(fileName);
        QFile::copy(ressourcePath, absPath);
    }
}
void PatchGeneratorBase::loadGenerators()
{
    for (auto &fileName: _directory.entryList()) {
        if (fileName.startsWith('.') || fileName.endsWith(".py")) {
            continue;
        }
        QString absPath = _directory.absoluteFilePath(fileName);
        PatchGenerator *gen = new PatchGenerator(absPath, fileName);
        if (gen->isValid())
            _generators.append(gen);
        else {
            QString title = TR("Failed to load patch generator '") + absPath + "'";

            QMessageBox::warning(
                        0,
                        title,
                        title + "\n\n" + gen->error(),
                        QMessageBox::Ok);
            delete gen;
        }
    }
}
