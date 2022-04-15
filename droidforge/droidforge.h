#ifndef DROIDFORGE_H
#define DROIDFORGE_H

#include "droidfirmware.h"
#include "undohistory.h"
#include "patchparser.h"

class DroidForge
{
    DroidFirmware firmware;
    UndoHistory undoHistory;
    PatchParser parser;
    Patch *patch;
    QString filename; // of loaded patch

public:
    DroidForge();
    ~DroidForge();
    bool loadPatch(QString filename);
    Patch *getPatch() { return patch; };
    void registerEdit(QString name);
    bool undoPossible();
    void undo();
};

extern DroidForge *the_forge;
extern DroidFirmware *the_firmware;


#endif // DROIDFORGE_H
