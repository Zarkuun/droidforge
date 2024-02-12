#include "patch.h"
#include "atomcable.h"
#include "atomnumber.h"
#include "droidfirmware.h"
#include "modulebuilder.h"
#include "registerlabels.h"
#include "globals.h"

#include <QFileInfo>
#include <QSettings>

Patch::Patch()
    : sectionIndex(0)
{
}
Patch::~Patch()
{
    clear();
}
void Patch::clear()
{
    title = "";
    description.clear();
    libraryMetaData = "";
    registerLabels.clear();
    controllers.clear();
    for (auto section: sections)
        delete section;
    sections.clear();
    sectionIndex = 0;
    for (auto problem: problems)
        delete problem;
    problems.clear();
}

Patch *Patch::clone() const
{
    Patch *newPatch = new Patch();
    cloneInto(newPatch);
    return newPatch;
}
void Patch::cloneInto(Patch *otherPatch) const
{
    otherPatch->clear();
    otherPatch->title = title;
    otherPatch->description = description;
    otherPatch->libraryMetaData = libraryMetaData;
    otherPatch->registerLabels = registerLabels;
    otherPatch->registerLabels.detach();
    otherPatch->controllers = controllers;
    for (auto section: sections)
        otherPatch->sections.append(section->clone());
    otherPatch->sectionIndex = sectionIndex;
}
void Patch::addSection(PatchSection *section)
{
    sections.append(section);
}
void Patch::insertSection(int index, PatchSection *section)
{
    sections.insert(index, section);
    sectionIndex = index;
}
void Patch::mergeSections(int indexa, int indexb)
{
    PatchSection *sectiona = sections[indexa];
    PatchSection *sectionb = sections[indexb];
    for (auto circuit: sectionb->getCircuits())
        sectiona->addCircuit(circuit->clone());
    removeSection(indexb);
    sectionIndex = qMin(indexa, indexb);
}
void Patch::removeSection(int index)
{
    delete sections[index];
    sections.remove(index);
    sectionIndex = qMin(sectionIndex, sections.size() - 1);
}
void Patch::integratePatch(const Patch *snippet)
{
    int index;
    if (isEmpty())
        index = 0;
    else
        index = sectionIndex + 1;
    int startIndex = index;

    for (auto section: snippet->sections)
    {
        PatchSection *clonedSection = section->clone();
        if (snippet->numSections() == 1 && clonedSection->getTitle().isEmpty()) {
            clonedSection->setTitle(snippet->getTitle());
        }
        insertSection(index++, clonedSection);
    }
    sectionIndex = startIndex; // move user directly to new section
}
void Patch::reorderSections(int fromindex, int toindex)
{
    PatchSection *moved = sections[fromindex];
    sections.remove(fromindex);
    sections.insert(toindex, moved);
}
void Patch::duplicateCurrentCircuit()
{
    currentSection()->duplicateCurrentCircuit();
}
void Patch::swapControllersSmart(int fromindex, int toindex)
{
    QStringList newControllers;
    for (qsizetype i=0; i<controllers.size(); i++) {
        if (i == fromindex)
            newControllers.append(controllers[toindex]);
        else if (i == toindex)
            newControllers.append(controllers[fromindex]);
        else
            newControllers.append(controllers[i]);
    }
    controllers = newControllers;
    swapControllerNumbers(fromindex+1, toindex+1);
    registerLabels.swapControllerNumbers(fromindex+1, toindex+1);
}

void Patch::moveControllerSmart(int fromIndex, int toIndex)
{
    if (fromIndex < toIndex) {
        for (int i=fromIndex; i<toIndex; i++)
            swapControllersSmart(i, i+1);
    }
    else {
        for (int i=fromIndex; i>toIndex; i--)
            swapControllersSmart(i, i-1);
    }
}
void Patch::removeController(int index)
{
    registerLabels.removeController(index + 1);
    registerLabels.shiftControllerNumbers(index + 1, -1);
    shiftControllerNumbers(index + 1, -1);
    controllers.remove(index);
}
void Patch::addDescriptionLine(const QString &line)
{
    description.append(line);
}
void Patch::addDescriptionLines(const QStringList &list)
{
    description += list;
}
void Patch::setDescription(const QString &d)
{
    description = d.split('\n');
    if (d.endsWith("\n"))
        description.removeLast();
}
void Patch::addRegisterLabel(register_type_t registerType, unsigned controller, unsigned g8, unsigned number, const QString &shorthand, const QString &comment)
{
    AtomRegister atom(registerType, controller, g8, number);
    RegisterLabel rc{shorthand, comment};
    registerLabels[atom] = rc;
}
void Patch::moveRegistersToOtherControllers(int controllerIndex, RegisterList &registers)
{
    unsigned controller = controllerIndex + 1;

    RegisterList allRegisters;
    collectAvailableRegisterAtoms(allRegisters);

    RegisterList usedRegisters;
    collectUsedRegisterAtoms(usedRegisters);

    RegisterList remapFrom;
    RegisterList remapTo;
    RegisterList remapped;

    // Loop through all registers to be remapped
    for (auto& toRemap: registers)
    {
        // Loop through all candidate registers
        for (auto &candidate: allRegisters) {
            if (candidate.getController() == controller)
                continue; // Don't remap to the controller we want to free

            if (toRemap.getRegisterType() != candidate.getRegisterType())
                continue; // not suitable

            if (usedRegisters.contains(candidate))
                continue; // not free

            if (toRemap.getRegisterType() == candidate.getRegisterType())
            {
                usedRegisters.append(candidate);
                remapFrom.append(toRemap);
                remapTo.append(candidate);
                remapped.append(toRemap);
                break;
            }
        }
    }

    for (auto& atom: remapped)
        registers.removeAll(atom);

    // Apply this remapping
    for (unsigned i=0; i<remapFrom.size(); i++)
        remapRegister(remapFrom[i], remapTo[i]);
}
QString Patch::getDescription() const
{
    if (description.empty())
        return "";
    else
        return description.join("\n") + "\n";
}
RegisterLabel Patch::registerLabel(AtomRegister atom) const
{
    if (registerLabels.contains(atom))
        return registerLabels[atom];
    else
        return RegisterLabel();
}
void Patch::setRegisterLabel(AtomRegister atom, RegisterLabel label)
{
    registerLabels[atom] = label;
}
void Patch::removeRegisterLabel(AtomRegister atom)
{
    registerLabels.remove(atom);
}
unsigned Patch::numCircuits() const
{
    unsigned num = 0;
    for (auto& section: sections)
        num += section->numCircuits();
    return num;
}
const Circuit *Patch::currentCircuit() const
{
    return currentSection()->currentCircuit();
}
void Patch::moveSection(int fromIndex, int toIndex)
{
    PatchSection *section = sections[fromIndex];
    sections.remove(fromIndex);
    sections.insert(toIndex, section);
}
const Atom *Patch::currentAtom() const
{
    return currentSection()->currentAtom();
}
void Patch::setCursorTo(int section, const CursorPosition &pos)
{
    sectionIndex = section;
    currentSection()->setCursor(pos);
}
bool Patch::moveCursorForward(bool autoUnfold)
{
    bool wrapped = false;

    int circuitNr = 0;
    int row = ROW_CIRCUIT;
    int column = 0;

    if (!currentSection()->isEmpty()) {
        // Move the cursor to the next possible atom, jack, circuit, whatever
        CursorPosition pos = currentSection()->cursorPosition();
        circuitNr = pos.circuitNr;
        row = pos.row;
        column = pos.column;
        if (row == ROW_CIRCUIT) {
            if (currentCircuit()->hasComment())
                row = ROW_COMMENT;
            else
                row = 0;
            column = 0;
        }
        else if (row == ROW_COMMENT) {
            row = 0;
            column = 0;
        }
        else {
            JackAssignment *ja = currentSection()->currentJackAssignment();
            column ++;
            if (column > ja->numColumns()) {
                row ++;
                column = 0;
            }
        }

        if (row >= currentCircuit()->numJackAssignments()) {
            circuitNr ++;
            row = ROW_CIRCUIT;
            column = 0;
        }
    }
    else
        circuitNr = 0;

    if (circuitNr >= (int) currentSection()->numCircuits()) {
        circuitNr = 0;
        row = ROW_CIRCUIT;
        column = 0;
        sectionIndex++;
    }

    if (sectionIndex >= numSections()) {
        circuitNr = 0;
        row = ROW_CIRCUIT;
        column = 0;
        sectionIndex = 0;
        wrapped = true;
    }

    if (autoUnfold)
        currentSection()->setCursor(CursorPosition(circuitNr, row, column));
    else
        currentSection()->setCursorNoUnfold(CursorPosition(circuitNr, row, column));
    return wrapped;
}
void Patch::moveCursorBackward(bool autoUnfold)
{
    // Move the cursor to the previous possible atom, jack, circuit, whatever
    CursorPosition pos = currentSection()->cursorPosition();
    int circuitNr = pos.circuitNr;
    int row = pos.row;
    int column = pos.column;

    if (row == ROW_CIRCUIT) {
        circuitNr--;
        if (circuitNr < 0) {
            sectionIndex --;
            if (sectionIndex < 0)
                sectionIndex = numSections() - 1;
            circuitNr = currentSection()->numCircuits() - 1;
        }
        const Circuit *circuit = currentSection()->circuit(circuitNr);
        row = circuit->numJackAssignments() - 1;
        if (row >= 0) {
            const JackAssignment *ja = circuit->jackAssignment(row);
            column = ja->numColumns();
        }
    }
    else if (row == ROW_COMMENT) {
        row --;
        column = 0;
    }
    else {
        // JackAssignment *ja = currentSection()->currentJackAssignment();
        column --;
        if (column < 0) {
            row --;
            if (row < 0) {
                const Circuit *circuit = currentSection()->currentCircuit();
                column = 0;
                if (!circuit->hasComment() && row == ROW_COMMENT)
                    row--;
            }
            else {
                const Circuit *circuit = currentSection()->circuit(circuitNr);
                const JackAssignment *ja = circuit->jackAssignment(row);
                column = ja->numColumns();
            }
        }
    }

    if (autoUnfold)
        currentSection()->setCursor(CursorPosition(circuitNr, row, column));
    else
        currentSection()->setCursorNoUnfold(CursorPosition(circuitNr, row, column));
    return;
}
unsigned Patch::searchHitPosition(const QString &text, unsigned *count)
{
    *count = 0;
    unsigned position = 0;
    int startSectionIndex = sectionIndex;
    CursorPosition startCursor = currentSection()->cursorPosition();

    sectionIndex = 0;
    currentSection()->setCursor(CursorPosition(0, ROW_CIRCUIT, 0));
    currentSection()->sanitizeCursor();
    while (true) {
        if (currentSection()->searchHitAtCursor(text)) {
            (*count)++;
            if (startSectionIndex == sectionIndex &&
                    startCursor == currentSection()->cursorPosition()) {
                position = *count;
            }
        }
        if (moveCursorForward(false /* autoUnfold */))
            break; // wrapped around
    }
    sectionIndex = startSectionIndex;
    currentSection()->setCursor(startCursor); // unfolds if neccessary
    return position;
}
void Patch::setTitle(const QString &newTitle)
{
    title = newTitle;
}
QStringList Patch::allCables() const
{
    QStringList cables;
    for (qsizetype i=0; i<sections.length(); i++)
        sections[i]->collectCables(cables);
    cables.removeDuplicates();
    cables.sort();
    return cables;
}
void Patch::renameCable(const QString &oldName, const QString &newName)
{
    for (auto atom: *this) {
        if (atom->isCable()) {
            AtomCable *ac = (AtomCable *)atom;
            if (ac->getCable() == oldName)
                ac->setCable(newName);
        }
    }
}
void Patch::rewriteCableNames(const QString &remove, const QString &insert, RewriteCablesDialog::mode_t mode)
{
    if (currentSection()->getSelection())
        currentSection()->rewriteSelectedCableNames(remove, insert, mode);
    else {
        for (auto atom: *this) {
            atom->rewriteCableNames(remove, insert, mode);
        }
    }
}
void Patch::compressCables(QMap<QString,QString> *mapping)
{
    unsigned nextId = 0;
    QStringList cables = allCables();
    for (auto& cable: cables) {
        QString newName = createCompressedCableName(nextId++);
        while (cables.contains(newName))
           newName = createCompressedCableName(nextId++);
        renameCable(cable, newName);
        if (mapping)
            (*mapping)[cable] = newName; // for status dumps
    }
}
void Patch::clearBookmarks()
{
    for (auto section: sections)
        section->clearBookmarks();
}

bool Patch::findBookmark(int *sectionNr, CursorPosition *pos)
{
    for (int i=0; i<sections.count(); i++) {
        if (sections[i]->findBookmark(pos)) {
            *sectionNr = i;
            return true;
        }
    }
    return false;
}
bool Patch::hasBookmark()
{
    int section;
    CursorPosition pos;
    return findBookmark(&section, &pos);
}
void Patch::setBookmark()
{
    currentSection()->setBookmark();
}
QString Patch::createCompressedCableName(unsigned id)
{
    QString name;
    // make a system of 26
    do {
        unsigned digit = id % 26;
        QChar c = char(65 + digit);
        name += c;
        id = id / 26;
    } while (id > 0);
    return name;
}
void Patch::findCableConnections(const QString &cable, int &asInput, int &asOutput) const
{
    for (auto section: sections)
        section->findCableConnections(cable, asInput, asOutput);
}
bool Patch::cableExists(const QString &cable) const
{
    int asInput = 0;
    int asOutput = 0;
    findCableConnections(cable, asInput, asOutput);
    return asInput > 0 || asOutput > 0;
}
QString Patch::freshCableName() const
{
    QString name = TR("CABLE1");
    while (cableExists(name))
        name = AtomCable::nextCableName(name);
    return name;
}
void Patch::collectUsedRegisterAtoms(RegisterList &sl, bool skipOverlayedControls) const
{
    // We could use the iterator but that has no const
    // version. Copying the whole iterator is no option.
    // Maybe there is a way to do that with templates.
    // If you feel like doing that, send me a pull request.
    for (auto section: sections)
        section->collectRegisterAtoms(sl, skipOverlayedControls);
}
bool Patch::registerUsed(AtomRegister reg)
{
    for (auto &atom: *this) {
        if (atom->isRegister()) {
            AtomRegister *areg = (AtomRegister *)atom;
            if (*areg == reg)
                return true;
        }
    }
    return false;
}

bool Patch::registerIsOutputOnly(AtomRegister reg) const
{
    switch (reg.getRegisterType()) {
         case REGISTER_INPUT:
         case REGISTER_POT:
         case REGISTER_BUTTON:
         case REGISTER_SWITCH:
         case REGISTER_ENCODER:
             return false;

         case REGISTER_NORMALIZE:
         case REGISTER_OUTPUT:
         case REGISTER_LED:
         case REGISTER_RGB_LED:
         case REGISTER_EXTRA:
             return true;

         case REGISTER_GATE:
             return reg.getNumber() >= 9; // TODO master18: depends
    }
   return false; // never reached
}
bool Patch::controlUsed(AtomRegister reg)
{
    for (auto &atom: *this) {
        if (atom->isRegister()) {
            AtomRegister *areg = (AtomRegister *)atom;
            if (areg->isRelatedTo(reg))
                return true;
        }
    }
    return false;
}
void Patch::collectAvailableRegisterAtoms(RegisterList &rl) const
{
    ModuleBuilder::allRegistersOf("master", 0, 0, rl);
    for (unsigned g=1; g<=MAX_NUM_G8S; g++)
        ModuleBuilder::allRegistersOf("g8", 0, g, rl);

    ModuleBuilder::allRegistersOf("x7", 0, 0, rl);
    unsigned controllerNumber = 1;

    for (auto &controller: controllers) {
        ModuleBuilder::allRegistersOf(controller, controllerNumber, 0, rl);
        controllerNumber++;
    }
}
unsigned Patch::numProblemsInSection(int i) const
{
    unsigned count = 0;
    for (auto problem: problems) {
        if (problem->getSection() == i)
            count ++;
    }
    return count;
}
void Patch::updateProblems()
{
    for (auto problem: problems)
        delete problem;
    problems.clear();

    // Collect problems locally in the sections
    int sectionNr=0;
    for (auto section: sections) {
        auto sectionProblems = section->collectProblems(this);
        for (auto problem: sectionProblems)
            problem->setSection(sectionNr);
        problems += sectionProblems;
        sectionNr++;
    }

    // Look for outputs (O) or normalizations (N) that are used twice as output.
    // LEDs are fine! Since many circuits work with select and allow
    // overloading of LEDs.
    RegisterList usedOutputs;
    for (auto it = begin(); it != end(); ++it)
    {
        const Atom *atom = *it;
        if (!atom->isRegister() || !it.isOutput())
            continue;

        const AtomRegister *reg = (const AtomRegister *)atom;
        const CursorPosition &pos = it.cursorPosition();
        const PatchSection *sec = section(it.sectionIndex());
        const Circuit *circuit = sec->circuit(pos.circuitNr);
        const JackAssignment *ja = circuit->jackAssignment(pos.row);
        if (!ja->isDisabled())
        {
            if ((reg->getRegisterType() == REGISTER_OUTPUT ||
                reg->getRegisterType() == REGISTER_GATE ||
                 reg->getRegisterType() == REGISTER_NORMALIZE) &&
                usedOutputs.contains(*reg))
            {
                PatchProblem *prob = new PatchProblem(pos.row, pos.column,
                                                      TR("Duplicate usage of %1 as output").arg(reg->toString()));
                prob->setCircuit(pos.circuitNr);
                prob->setSection(it.sectionIndex());
                problems.append(prob);
            }
            usedOutputs.append(*reg);
        }
    }

    // Check usage of output registers as inputs, if they are not also used
    // as outputs
    for (auto it = begin(); it != end(); ++it)
    {
        const Atom *atom = *it;
        if (!atom->isRegister() || it.isOutput())
            continue;
        const AtomRegister *reg = (const AtomRegister *)atom;
        if (!registerIsOutputOnly(*reg))
            continue; // we are looking for output registers
        if (!usedOutputs.contains(*reg)) {
            const CursorPosition &pos = it.cursorPosition();
            const PatchSection *sec = section(it.sectionIndex());
            const Circuit *circuit = sec->circuit(pos.circuitNr);
            const JackAssignment *ja = circuit->jackAssignment(pos.row);
            if (!ja->isDisabled())
            {
                PatchProblem *prob = new PatchProblem(pos.row, pos.column,
                                                      TR("Output register %1 is just used as an input.").arg(reg->toString()));
                prob->setCircuit(pos.circuitNr);
                prob->setSection(it.sectionIndex());
                problems.append(prob);
            }
        }
    }


    // Check memory consumption of circuits, also their count
    unsigned usedMemory = 0;
    unsigned availableMemory = the_firmware->availableMemory();
    unsigned sectionIndex = 0;
    QMap<QString, unsigned> circuitCounts;

    for (auto section: sections) {
        unsigned circuitNr = 0;
        for (auto circuit: section->getCircuits()) {
            if (!circuit->isDisabled()) {
                if (usedMemory + circuit->memoryFootprint() > availableMemory) {
                    PatchProblem *prob = new PatchProblem(
                                ROW_CIRCUIT, 0,
                                TR("This circuit exceeds the available memory."));
                    prob->setCircuit(circuitNr);
                    prob->setSection(sectionIndex);
                    problems.append(prob);
                }
                usedMemory += circuit->memoryFootprint();
                QString c = circuit->getName();
                if (!circuitCounts.contains(c))
                    circuitCounts[c] = 0;
                circuitCounts[c] ++;
                if (circuitCounts[c] == 255) { // one too much
                    if (the_firmware->circuitIsPersisted(c)) {
                        PatchProblem *prob = new PatchProblem(
                                    ROW_CIRCUIT, 0,
                                    TR("You have too many circuits of this type. The maximum number is 255."));
                        prob->setCircuit(circuitNr);
                        prob->setSection(sectionIndex);
                        problems.append(prob);
                    }
                }
            }
            circuitNr++;
        }
        sectionIndex++;
    }
}
QString Patch::problemAt(int section, const CursorPosition &pos)
{
    CursorPosition realPos = sections[section]->canonizedCursorPosition(pos);
    for (auto problem: problems) {
        if (problem->isAt(section, realPos))
            return problem->getReason();
    }
    return "";
}
bool Patch::registerAvailable(AtomRegister ar) const
{
    register_type_t regType = ar.getRegisterType();
    unsigned max;

    if (ar.isControl()) {
        unsigned c = ar.getController();
        if (c > controllers.count())
            return false;
        QString name = controllers[c-1];
        max = the_firmware->numControllerRegisters(name, regType);
    }
    else
        max = the_firmware->numGlobalRegisters(regType);

    return ar.getNumber() >= 1 && ar.getNumber() <= max;
}
unsigned Patch::memoryFootprint(QStringList &breakdown) const
{
    unsigned memory = 0;

    // If the patch is completely empty, Droid will replace it with
    // one that consists of the sole circuit "[droid]". Because it
    // cannot save an empty patch to the flash.
    if (numCircuits() == 0 && controllers.size() == 0) {
        memory = the_firmware->circuitMemoryFootprint("droid");
    }
    else {
        unsigned byCircuits = 0;
        for (auto section: sections)
            byCircuits += section->memoryFootprint();
        memory += byCircuits;
        memory -= 12; // Somehow neccessary in blue-2
        breakdown.append(TR("%1 bytes are needed by %2 circuits.")
                         .arg(byCircuits).arg(numCircuits()));

        unsigned byControllers = 0;
        for (const QString &controller: controllers)
            byControllers += the_firmware->controllerMemoryFootprint(controller);
        memory += byControllers;
        breakdown.append(TR("%1 bytes are needed by %2 controllers.")
                         .arg(byControllers).arg(controllers.count()));
    }

    // We must assume an attached X7. The X7 need 1k RAM just because
    // it's attached.
    unsigned byX7 = the_firmware->controllerMemoryFootprint("x7");
    if (((Patch *)this)->needsX7())
        breakdown.append(TR("%1 bytes are used by the X7.").arg(byX7));
    else
        breakdown.append(TR("%1 bytes could be used by a potential X7.").arg(byX7));
    memory += byX7;


    unsigned numConstants = ((Patch *)this)->countUniqueConstants();
    memory += numConstants * 4;
    breakdown.append(TR("%1 bytes are used by %2 unique constants.")
                     .arg(numConstants * 4).arg(numConstants));

    unsigned numCables = ((Patch *)this)->countUniqueCables();
    memory += numCables * 8;
    breakdown.append(TR("%1 bytes are used by %2 unique Cables.")
                     .arg(numCables * 8).arg(numCables));
    return memory;
}
unsigned Patch::countUniqueCables()
{
    QSet<QString> cables;
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isCable()) {
            QString name = ((AtomCable *)atom)->getCable();
            cables.insert(name);
        }
    }
    return cables.count();
}

unsigned Patch::countUniqueConstants()
{
    // This algo is quite complex. Please have a look at the Droid firmware
    // thedroid.cc for the mirrored code that is the basis for this.
    QSet<float> constants;
    constants.insert(0.0);
    constants.insert(1.0);
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isNumber()) {
            AtomNumber *an = (AtomNumber *)atom;
            float number = an->getNumber();
            if (number == 0.0)
                continue;
            constants.insert(number);
            constants.insert(-number);
            if (an->isFraction()) {
                constants.insert(1.0 / number);
                constants.insert(1.0 / -number);
            }
        }
    }
    return constants.count();
}

unsigned Patch::neededG8s()
{
    unsigned highest_g8 = 0;
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isRegister()) {
            AtomRegister *reg = (AtomRegister *)atom;
            if (reg->getRegisterType() == REGISTER_GATE &&
                reg->getNumber() < 9 /* don't confuse with X7 */) {
                highest_g8 = 1;
            }
        }
    }
    return highest_g8;
}
bool Patch::needsX7()
{
    // First check for G9, G10, G11, G12
    // and the R registers of the X7
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isRegister()) {
            AtomRegister *reg = (AtomRegister *)atom;
            if (reg->needsX7())
                return true;
        }
    }

    for (auto section: sections)
        if (section->needsX7())
            return true;

    return false;
}
void Patch::remapRegister(AtomRegister from, AtomRegister to)
{
    for (auto &atom: *this) {
        if (atom->isRegister()) {
            AtomRegister *reg = (AtomRegister *)atom;
            if (*reg == from)
                *reg = to;
        }
    }
    registerLabels.remapRegister(from, to);
}
void Patch::swapRegisters(AtomRegister regA, AtomRegister regB)
{
    for (auto &atom: *this) {
        if (atom->isRegister()) {
            AtomRegister *reg = (AtomRegister *)atom;
            if (*reg == regA)
                *reg = regB;
            else if (*reg == regB)
                *reg = regA;
        }
    }
    registerLabels.swapRegisters(regA, regB);
}
void Patch::removeRegisterReferences(RegisterList &rl)
{
    for (auto section: sections)
        section->removeRegisterReferences(rl);
}
void Patch::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (auto atom: *this) {
        if (atom->isRegister()) {
            AtomRegister *areg = (AtomRegister *)atom;
            areg->swapControllerNumbers(fromNumber, toNumber);
        }
    }
}
void Patch::shiftControllerNumbers(int number, int by)
{
    for (auto atom: *this) {
        if (atom->isRegister()) {
            AtomRegister *areg = (AtomRegister *)atom;
            areg->shiftControllerNumbers(number, by);
        }
    }
}
void Patch::duplicateController(int index, bool withLabels)
{
    QString name = controllers[index];
    controllers.append(name);
    moveControllerSmart(controllers.count()-1, index+1);
    if (withLabels)
        registerLabels.copyControllerLabels(index+1, index+2);
}
QString Patch::toString() const
{
    QString s;
    if (!title.isEmpty())
        s += "# " + title + "\n";

    if (!libraryMetaData.isEmpty())
        s += "# LIBRARY: " + libraryMetaData + "\n";

    if (!description.isEmpty()) {
        if (!s.isEmpty())
            s += "\n";
        for (qsizetype i=0; i<description.length(); i++) {
            s += "# " + description[i] + "\n";
        }
        s += "\n";
    }

    s += registerLabels.toString();

    for (qsizetype i=0; i<controllers.length(); i++)
        s += "[" + controllers[i] + "]\n";

    if (!s.isEmpty())
        s += "\n";

    QString sectionPart;
    for (qsizetype i=0; i<sections.length(); i++)
        sectionPart += sections[i]->toString(i == 0 /* supress empty header */);

    // If the patch does not have a title and also no description,
    // the first comment of the section part would be interpreted
    // as title. In order to avoid that we introduce an empty line
    // if that is the situation.
    if (s.isEmpty() && sectionPart.startsWith("#"))
        s += "\n";
    s += sectionPart;

    while (s.endsWith("\n\n"))
        s.chop(1);
    return s;
}

QString Patch::toCleanString() const
{
    QString s;
    for (qsizetype i=0; i<controllers.length(); i++)
        s += "[" + controllers[i] + "]\n";
    if (controllers.length())
        s += "\n";
    for (qsizetype i=0; i<sections.length(); i++) {
        s += sections[i]->toCleanString();
    }
    if (s.endsWith("\n\n"))
        s.chop(1);

    return s;

}
QString Patch::toBare() const
{
    QString s;
    for (qsizetype i=0; i<controllers.length(); i++)
        s += "[" + controllers[i] + "]\n";
    for (qsizetype i=0; i<sections.length(); i++)
        s += sections[i]->toBare();
    return s;
}
QString Patch::toCompressed() const
{
    QSettings settings;
    bool renameCables = settings.value("compression/rename_cables", false).toBool();
    bool removeEmptyLines = settings.value("compression/remove_empty_lines", false).toBool();
    QString source;
    if (renameCables) {
         Patch *patch = clone();
         patch->compressCables();
         source = patch->toString();
         delete patch;
    }
    else
        source = toString();

    // Remove spaces and tabs
    source.remove(' ');
    source.remove('\t');

    // Remove comments
    static QRegularExpression comment("#.*");
    source.remove(comment);

    // Remove empty lines
    if (removeEmptyLines) {
        while (source.startsWith("\n"))
            source.remove(0, 1);
        while (source.contains("\n\n"))
            source.replace("\n\n", "\n");
    }
    return source;
}
bool Patch::saveToFile(const QString filePath, bool compressed) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
          return false;
    QTextStream stream(&file);
    if (compressed)
        stream << toCompressed();
    else
        stream << toString();
    stream.flush();
    file.close();
    return stream.status() == QTextStream::Ok;
}
void Patch::iterator::moveToFirstAtom()
{
    atom = 0;
    for (nSection = 0; nSection < patch->sections.size(); nSection++)
    {
        section = patch->sections[nSection];
        for (nCircuit = 0; nCircuit < (int)section->numCircuits(); nCircuit++)
        {
            circuit = section->circuit(nCircuit);
            if (onlyEnabled && circuit->isDisabled())
                continue;

            for (nJack=0; nJack<circuit->numJackAssignments(); nJack++)
            {
                jackAssignment = circuit->jackAssignment(nJack);
                if (onlyEnabled && jackAssignment->isDisabled())
                    continue;

                for (nAtom=1; nAtom<=3; nAtom++) {
                    atom = jackAssignment->atomAt(nAtom);
                    if (atom)
                        return;
                }
            }
        }
    }
}
bool Patch::iterator::advance()
{
    while (true) {
        nAtom ++;
        if (nAtom > jackAssignment->numColumns()) {
            if  (!advanceJack()) { // sets nAtom to 0
                atom = 0;
                return false;
            }
            nAtom = 1;
        }
        if (jackAssignment->atomAt(nAtom)) {
            atom = jackAssignment->atomAt(nAtom);
            return true;
        }
    }
}
bool Patch::iterator::advanceJack()
{
    while (true) {
        if (nJack+1 < circuit->numJackAssignments()) {
            nJack ++;
            jackAssignment = circuit->jackAssignment(nJack);
            if (onlyEnabled && jackAssignment->isDisabled())
                return advanceJack();
            else
                return true;
        }
        else if (!advanceCircuit())
            return false;
        nJack = -1;
    }
}
bool Patch::iterator::advanceCircuit()
{
    while (true) {
        if (nCircuit+1 < (int)section->numCircuits()) {
            nCircuit ++;
            circuit = section->circuit(nCircuit);
            if (onlyEnabled && circuit->isDisabled())
                return advanceCircuit();
            else
                return true;
        }
        else if (!advanceSection())
            return false;
        nCircuit = -1;
    }
}
bool Patch::iterator::advanceSection()
{
    if (nSection+1 < patch->sections.size()) {
        nSection ++;
        section = patch->sections[nSection];
        return true;
    }
    else
        return false;
}
