#include "interactivepatchoperation.h"
#include "controllerremovaldialog.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QApplication>

#define tr(s) QCoreApplication::translate("Patch", s)


bool InteractivePatchOperation::interactivelyRemapRegisters(Patch *patch, Patch *otherpatch)
{
    // Phase 1: If the other patch defines controllers, we can add these
    // controllers to our patch (and shift all references, of course)
    const QStringList &controllers = otherpatch->allControllers();
    if (!controllers.isEmpty())
    {
        int reply = QMessageBox::question(
                    the_forge,
                    tr("Controllers"),
                    tr("The integrated patch contains controller definitions: %1. "
                       "Do you want to add these definitions to your patch?")
                       .arg(controllers.join(" ").toUpper()),
                    QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::Yes);

        if (reply == QMessageBox::Cancel)
            return false;

        else if (reply == QMessageBox::Yes) {
            int numExistingControllers = patch->numControllers();
            for (auto &c: controllers)
                patch->addController(c);
            otherpatch->shiftControllerNumbers(-1, numExistingControllers);
        }
    }

    // Phase 2: Remap non-existing or conflicting registers.
    RegisterList availableRegisters;
    patch->collectAvailableRegisterAtoms(availableRegisters);
    RegisterList occupiedRegisters;
    patch->collectUsedRegisterAtoms(occupiedRegisters);
    RegisterList neededRegisters;
    otherpatch->collectUsedRegisterAtoms(neededRegisters);
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
                    tr("Register conflicts"),
                    tr("Some of the register references in the integrated patch either do not exist in your "
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
                otherpatch->remapRegister(remapFrom[i], remapTo[i]);

            // Phase 2b: Remaining un remapped registers
            if (!atomsToRemap.isEmpty()) {
                int reply = QMessageBox::question(
                            the_forge,
                            tr("Register conflicts"),
                            tr("For some register references I could not find a valid replacement in your patch. "
                               "Shall I remove these references (otherwise I would just leave them as "
                               "they are and you check yourselves later)?\n\n%1").arg(atomsToRemap.toString()),
                            QMessageBox::Cancel | QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes);

                if (reply == QMessageBox::Cancel)
                    return false;

                else if (reply == QMessageBox::Yes) {
                    otherpatch->removeRegisterReferences(
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
