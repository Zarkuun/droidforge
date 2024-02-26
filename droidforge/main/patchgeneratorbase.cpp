#include "patchgeneratorbase.h"
#include "globals.h"
#include "patchgenerator.h"

#include <QMessageBox>
#include <QMessageBox>
#include <QDirIterator>

PatchGeneratorBase *the_patch_generator_base = 0;

PatchGeneratorBase::PatchGeneratorBase(QDir directory)
    : _directory(directory)
{
    the_patch_generator_base = this;
}
bool PatchGeneratorBase::enableGenerators()
{
    return deployGenerators() &&  loadGenerators();
}
void PatchGeneratorBase::disableGenerators()
{
    for (auto gen: _generators)
    {
        delete gen;
    }
    _generators.clear();
}

bool PatchGeneratorBase::informAndAsk() const
{
    QString hint =  TR("You are going to enable the patch generators. "
                        "A patch generator is a little program that generates "
                        "a specialized Droid patch, based on a configuration that "
                        "you make in a dialog.\n\n"
                        ""
                        "These generators are written in the programming language "
                        "Python3, so just must have installed Python3 in order to "
                        "enable the generators.\n\n"
                        ""
                        "If you proceed and Python3 is not "
                        "yet installed, you will be guided through an installation "
                        "procedure by your operating system.\n\n"
                        "Do you want to proceeed?");

    return QMessageBox::question(
               0,
               "Enable patch generators?",
               hint,
               QMessageBox::Yes | QMessageBox::No,
               QMessageBox::Yes) == QMessageBox::Yes;
}
bool PatchGeneratorBase::deployGenerators()
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

    if (failed == "")
        return true;
    else {
        QMessageBox::warning(
            0,
            TR("Failed to install patch generators"),
            TR("The following patch generator files "
               "could not be installed:\n\n") + failed,
            QMessageBox::Ok);
        return false;
    }
}
bool PatchGeneratorBase::loadGenerators()
{
    bool oneLoaded = false;

    for (auto &fileName: _directory.entryList()) {
        if (fileName.startsWith('.') || fileName.endsWith(".py")) {
            continue;
        }
        QString absPath = _directory.absoluteFilePath(fileName);
        PatchGenerator *gen = new PatchGenerator(absPath, fileName);
        if (gen->isValid()) {
            _generators.append(gen);
            oneLoaded = true;
        }
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
    return oneLoaded;
}
