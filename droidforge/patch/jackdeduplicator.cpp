#include "jackdeduplicator.h"
#include "droidfirmware.h"
#include "jackassignmentinput.h"
#include "atomnumber.h"
#include "globals.h"

JackDeduplicator::JackDeduplicator(bool dedup)
    : deduplicate(dedup)
    , savedBytes(0)
{
    jacktableOffset = the_firmware->initialJacktableSize();
}

QString JackDeduplicator::processJackAssignment(const JackAssignment *ja)
{
    // Undefined jacks are ignored by the DROID master, so
    // no track keeping within the jack table is neccessary.
    if (ja->isUndefined()) {
        return "";
    }

    QString ramHint = the_firmware->jackRamHint(circuitName, ja->jackName());

    // The size of a jack depends on its type. This code here seems like a bit
    // of a hack, but even if we would transfer the actual sizes in the firmware
    // file, it still would be a hack since we have to mimick all of the optimizations
    // of the master here anyway.

    unsigned size;
    bool sharable = false;

    if (ramHint == "taptempo_input")
        size = 40;
    else if (ramHint == "trigger_input")
        size = 20;
    else if (ramHint == "input")
    {
        size = 16; // can be optimized into 8 or 4
        sharable = true;
    }
    else if (ramHint == "trigger_output")
        size = 8;
    else if (ramHint == "output")
        size = 4;
    else {
        // This cannot be reached, since the list of all possible ram hints is contained
        // in the firmware file and that file is part of the Forge source code.
        // At least this line cannot be reached by a Droid master's firmware not matching the Forge.

        // This line is reached if in the firmware definition file there
        // is a RAM hint that's different from the onces above. That means
        // that the JSON file is somehow corrupted. There are no other types
        // of RAM hints. So even if the case is a valid code path, we ignore
        // it and handle it like an unknown jack.
        return "";
    }

    // The master does some optimizations. In particular it detects "simple" inputs.
    // These are inputs without offset and attenuator. Of the form foo = bar (no + and *).
    // These are only done for inputs

    // The optimizations must match the code in parser.cc from the Droid firmware.
    if (ja->isInput() && size == 16 /* no trigger/taptempo */)
    {
        JackAssignmentInput *jai = (JackAssignmentInput *)ja;
        // All optimizations require that there is no * and +, just one number
        if (jai->isSimple()) { // No + and *
            size = 8;
            // The special values 0 and 1 have classes that don't need
            // any ram (other than 4 bytes for the virtual table)
            Atom *atom = jai->getAtom(0);
            if (atom->isNumber()) {
                AtomNumber *an = (AtomNumber *)atom;
                auto number = an->getNumber();
                if (number == 0.0 || number == 1.0)
                    size = 4; // special optimization
            }
        }
    }

    if (sharable && deduplicate) {
        JackAssignmentInput *jai = (JackAssignmentInput *)ja;
        QString value = jai->valueToCanonicalString();
        // Now comes the magic: If this values has appeared somewhere in the patch
        // already, we put "@124" there instead of the value, where @124 is the
        // offset in the jack table where a jack for this value has been created.
        if (sharedValues.contains(value)) {
            savedBytes += size;
            return QString("@") + QString::number(sharedValues[value]);
        }
        else {
            sharedValues[value] = jacktableOffset;
        }
    }

    jacktableOffset += size;
    return ja->valueToString().replace(" ", "");;
}
