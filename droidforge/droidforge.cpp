#include "droidforge.h"

DroidForge *the_forge;
DroidFirmware *the_firmware;

DroidForge::DroidForge()
    : patch(0)
{
    the_forge = this;
    the_firmware = &firmware;
}


DroidForge::~DroidForge()
{
    if (patch)
        delete patch;
}


bool DroidForge::loadPatch(QString afilename)
{
    Patch newpatch;
    if (!parser.parse(afilename, &newpatch))
        return false;

    if (patch)
        delete patch;
    patch = newpatch.clone();

    filename = afilename;
    undoHistory.clear();
    undoHistory.snapshot("Load from file", patch);
    return true;
}


void DroidForge::registerEdit(QString name)
{
    undoHistory.snapshot(name, patch);
}


bool DroidForge::undoPossible()
{
    return undoHistory.size() > 1;
}


void DroidForge::undo()
{
    patch = undoHistory.undo();
    qDebug() << "Undo! Patch has now" << patch->toString().size() << "bytes";
}
