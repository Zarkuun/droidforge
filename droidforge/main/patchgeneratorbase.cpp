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
    QString failed;

    QDirIterator it(":pg");
    while (it.hasNext()) {
        QString ressourcePath = it.next();
        auto parts = ressourcePath.split("/");
        QString fileName = parts[1];
        QString absPath = _directory.absoluteFilePath(fileName);
        if (QFile::exists(absPath))
            QFile::remove(absPath);
        if (!QFile::copy(ressourcePath, absPath))
            failed += absPath + "\n";
    }

    if (failed != "") {
        QMessageBox::warning(
            0,
            TR("Failed to install patch generators"),
            TR("The following patch generator files "
               "could not be installed:\n\n") + failed,
            QMessageBox::Ok);
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
