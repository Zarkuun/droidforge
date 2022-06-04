#include "patchview.h"
#include "controllerremovaldialog.h"
#include "mainwindow.h"
#include "globals.h"

#include <QMessageBox>

PatchView::PatchView(PatchEditEngine *patch)
    : patch(patch)
{
}

PatchSection *PatchView::section()
{
    return patch->currentSection(); // patch never 0, section never 0
}

const PatchSection *PatchView::section() const
{
    return patch->currentSection(); // patch never 0, section never 0
}


// TODO: Das hier ist doppelt. Rauswerfen. Das Original ist
// in PatchOperator
bool PatchView::interactivelyRemapRegisters(Patch *otherPatch, Patch *ontoPatch)
{
    // ontoPatch is that patch that the "otherPatch" want's to get integrated
    // to. In situations where the otherPatch does not have controllers, that
    // would be added to the target patch, ontoPatch is 0 and we can simply
    // use the "live" patch, since it will not be changed.
    if (ontoPatch == 0)
        ontoPatch = patch;

    // Phase 1: If the other patch defines controllers, we can add these
    // controllers to our patch (and shift all references, of course)
    const QStringList &controllers = otherPatch->allControllers();
    if (!controllers.isEmpty())
    {
        int reply = QMessageBox::question(
                    the_forge,
                    TR("Controllers"),
                    TR("The integrated patch contains controller definitions: %1. "
                       "Do you want to add these definitions to your patch?")
                       .arg(controllers.join(" ").toUpper()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            int numExistingControllers = ontoPatch->numControllers();
            for (auto &c: controllers)
                ontoPatch->addController(c);
            otherPatch->shiftControllerNumbers(-1, numExistingControllers);
        }
    }

    // TODO: Hier gibt's nochn Bug. Beispiel. Ich hab im otherpatch I1, I2 und I3.
    // Und im alten auch. Es wird aber nur I1->I4 gelegt. I2, und I3 sagt er kann
    // er nicht.

    // Phase 2: Remap non-existing or conflicting registers.
    RegisterList availableRegisters;
    ontoPatch->collectAvailableRegisterAtoms(availableRegisters);
    RegisterList occupiedRegisters;
    ontoPatch->collectUsedRegisterAtoms(occupiedRegisters);
    RegisterList neededRegisters;
    otherPatch->collectUsedRegisterAtoms(neededRegisters);
    RegisterList atomsToRemap;
    for (auto &reg: neededRegisters) {
        if (occupiedRegisters.contains(reg) || !availableRegisters.contains(reg)) {
            atomsToRemap.append(reg);
        }
        else
            occupiedRegisters.append(reg); // now occupied
    }
    if (atomsToRemap.count()) {
        int reply = QMessageBox::question(
                    the_forge,
                    TR("Register conflicts"),
                    TR("Some of the register references in the integrated patch either do not exist in your "
                       "current rack definition or are already occupied. Shall I try to find useful replacements "
                       "for those?\n\n%1").arg(atomsToRemap.toString()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            RegisterList remapFrom;
            RegisterList remapTo;
            RegisterList remapped;

            for (auto& toRemap: atomsToRemap) {
                for (auto &candidate: availableRegisters) {
                    if (occupiedRegisters.contains(candidate))
                        continue;
                    if (toRemap.getRegisterType() != candidate.getRegisterType())
                        continue;
                    remapFrom.append(toRemap);
                    remapTo.append(candidate);
                    occupiedRegisters.append(candidate);
                    remapped.append(toRemap);
                }
            }

            for (auto& atom: remapped)
                atomsToRemap.removeAll(atom);

            // Apply this remapping
            for (unsigned i=0; i<remapFrom.size(); i++)
                otherPatch->remapRegister(remapFrom[i], remapTo[i]);

            // Phase 2b: Remaining un remapped registers
            if (!atomsToRemap.isEmpty()) {
                int reply = QMessageBox::question(
                            the_forge,
                            TR("Register conflicts"),
                            TR("For some register references I could not find a valid replacement in your patch. "
                               "Shall I remove these references (otherwise I would just leave them as "
                               "they are and you check yourselves later)?\n\n%1").arg(atomsToRemap.toString()),
                            QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes);

                if (reply == QMessageBox::Cancel)
                    return false;

                else if (reply == QMessageBox::Yes) {
                    otherPatch->removeRegisterReferences(
                                atomsToRemap,
                                ControllerRemovalDialog::INPUT_REMOVE,
                                ControllerRemovalDialog::OUTPUT_REMOVE);
                }
            }
        }
    }

    // Phase 3: Cables
    // TODO: Alle Kabel des otherpatch sammeln. KOnflikte finden.
    // Wenn es welche gibt, fragen:
    // - umbenennen
    // - lassen
    // - abbrechen
    return true;
}

