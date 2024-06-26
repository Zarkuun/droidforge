#include "patch.h"
#include "QtWidgets/qmessagebox.h"
#include "atomcable.h"
#include "atomnumber.h"
#include "droidfirmware.h"
#include "modulebuilder.h"
#include "registerlabels.h"
#include "globals.h"
#include "tuning.h"
#include "utilities.h"


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
    labels.clear();
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
    otherPatch->labels = labels;
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
unsigned Patch::typeOfMaster() const
{
    /* Determine the type of master of this patch. This information
      either comes from the label "master" or it is autodetected.
      The auto detection is not very precise. It uses questions
      like "if I3 is used it cannot be a MASTER18". */
    auto result = labels.find("master");
    if (result != labels.end()) {
        unsigned value = result->toInt();
        if (value == 16 || value == 18)
            return value;
    }

    return 16; // This is the default
}

void Patch::setTypeOfMaster(unsigned new_type)
{
    labels["master"] = QString::number(new_type);
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
void Patch::setLabel(const QString &label, const QString &value)
{
    labels[label] = value;
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
QString Patch::labelsToString() const
{
    QString s;
    for (auto i = labels.cbegin(), end = labels.cend(); i != end; ++i) {
        QString l = i.key() + "=" + i.value() + ";";
        s += l;

    }
    if (s.endsWith(";"))
        s = s.chopped(1);
    return s;
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

         case REGISTER_GATE: {
             if (reg.getNumber() >= 9)
                 return true; // gates on X7
             else if (typeOfMaster() == 18 && reg.getG8Number() == 1)
                 return true;
             else
                 return false;
         }
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

    updateSectionProblems();
    updateRegisterProblems();
    updateMemoryProblems();
}

void Patch::updateMemoryProblems()
{
    // Check memory consumption of circuits, also their count
    unsigned availableMemory = the_firmware->availableMemory(typeOfMaster());
    unsigned usedMemory = usedRAMByControllers() +
                          4 * ((Patch *)this)->countUniqueConstants() +
                          8 * ((Patch *)this)->countUniqueCables();


    if (typeOfMaster() == 16)
        usedMemory += the_firmware->controllerUsedRAM("x7");

    QSettings settings;
    bool dedup = settings.value("compression/deduplicate_jacks", false).toBool();
    bool shorts = settings.value("compression/use_shortnames", false).toBool();
    JackDeduplicator jdd(dedup, shorts);

    QMap<QString, unsigned> circuitCounts;
    unsigned sectionIndex = 0;
    for (auto section: sections) {
        unsigned circuitNr = 0;
        for (auto circuit: section->getCircuits()) {
            if (!circuit->isDisabled()) {
                unsigned circuitMem = circuit->RAMUsage(jdd);
                if (usedMemory + circuitMem > availableMemory) {
                    QString error = TR("This circuit exceeds the available memory.");
                    if (!dedup)
                        error += TR(" Hint: activate \"Detect and share duplicate values for inputs and outputs\" in the preferences.");
                    PatchProblem *prob = new PatchProblem(ROW_CIRCUIT, 0, error);

                    prob->setCircuit(circuitNr);
                    prob->setSection(sectionIndex);
                    problems.append(prob);
                }
                usedMemory += circuitMem;
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
void Patch::updateSectionProblems()
{
    // Collect problems locally in the sections
    int sectionNr=0;
    for (auto section: sections) {
        auto sectionProblems = section->collectProblems(this);
        for (auto problem: sectionProblems)
            problem->setSection(sectionNr);
        problems += sectionProblems;
        sectionNr++;
    }
}
void Patch::updateRegisterProblems()
{
    unsigned master = typeOfMaster();

    // Invalid registers:
    // 1) Look for outputs (O) or normalizations (N) that are used twice as output.
    //    LEDs are fine! Since many circuits work with select and allow
    //    overloading of LEDs.
    // 2) Look for registers not present on the chosen master
    // 3) Collect additional registers used as output for a check below
    RegisterList usedOutputs;
    RegisterList usedAsOutput;
    for (auto it = begin(); it != end(); ++it)
    {
        const Atom *atom = *it;
        if (!atom->isRegister())
            continue; // we are only interested in registers

        const AtomRegister *reg = (const AtomRegister *)atom;
        const CursorPosition &pos = it.cursorPosition();
        const PatchSection *sec = section(it.sectionIndex());
        const Circuit *circuit = sec->circuit(pos.circuitNr);
        const JackAssignment *ja = circuit->jackAssignment(pos.row);
        if (ja->isDisabled())
            continue;

        if (ja->isOutput())
            usedAsOutput.append(*reg);

        // 1) Check registers used as ouput twice. We *don't* check LED registers
        // because it's quite common to use them twice - as a part of layered
        // menus with select and stuff.
        if  (it.isOutput() &&
            (reg->getRegisterType() == REGISTER_OUTPUT ||
            reg->getRegisterType() == REGISTER_NORMALIZE))
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

        // 2) Check for registers not available on the current master
        PatchProblem *prob = 0;
        if (master == 16) {
            if (reg->getG8Number() > 4)
                prob = new PatchProblem(pos.row, pos.column,
                                        TR("Invalid G8 number %1").arg(reg->getG8Number()));
        }
        else if (master == 18) {
            if (reg->getG8Number() == 1 && reg->getNumber() > 4)
            {
                prob = new PatchProblem(pos.row, pos.column,
                                        TR("Invalid gate number %1").arg(reg->getNumber()));
            }

            else if (reg->getRegisterType() == REGISTER_INPUT && reg->getNumber() > 2)
            {
                prob = new PatchProblem(pos.row, pos.column,
                                        TR("Invalid input number %1").arg(reg->getNumber()));
            }

            else if (reg->getRegisterType() == REGISTER_NORMALIZE)
            {
                prob = new PatchProblem(pos.row, pos.column,
                                        TR("The MASTER18 has no normalization registers"));
            }
        }
        if (prob) {
            prob->setCircuit(pos.circuitNr);
            prob->setSection(it.sectionIndex());
            problems.append(prob);
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
        if (!usedAsOutput.contains(*reg)) {
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

    if (typeOfMaster() == 18) {
        // Special handling for the fact that R5 ... R16 do
        // not exist on MASTER18
        if (regType == REGISTER_RGB_LED && ar.getNumber() >= 5 && ar.getNumber() <= 16)
            return false;
        else if (regType == REGISTER_EXTRA && ar.getNumber() == 1)
            return false;
    }

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
unsigned Patch::usedRAMByCircuits() const
{
    if (numCircuits() == 0 && controllers.size() == 0) {
        return the_firmware->circuitBaseRAM("droid");
    }
    else {
        // Count the RAM used by circuits (not counting their parameters)
        unsigned byCircuits = 0;
        for (auto section: sections)
            byCircuits += section->ramUsedByCircuits();
        return byCircuits;
    }
}
unsigned Patch::usedRAMByControllers() const
{
    unsigned byControllers = 0;
    for (const QString &controller: controllers)
        byControllers += the_firmware->controllerUsedRAM(controller);
    return byControllers;
}
unsigned Patch::usedRAM(QStringList &breakdown) const
{
    unsigned memory = 0;

    unsigned byCircuits = usedRAMByCircuits();
    if (numCircuits() || numControllers())
        breakdown.append(TR("%1 bytes are needed by %2 circuits.")
                             .arg(niceBytes(byCircuits)).arg(numCircuits()));
    else
        breakdown.append(TR("%1 bytes are needed by a hidden default circuit.")
                             .arg(niceBytes(byCircuits)));

    memory += byCircuits;

    unsigned byControllers = usedRAMByControllers();
    memory += byControllers;
    breakdown.append(TR("%1 bytes are needed by %2 controllers.")
                         .arg(niceBytes(byControllers)).arg(controllers.count()));

    // We must assume an attached X7. The X7 need 1k RAM just because
    // it's attached.
    if (typeOfMaster() == 16)
    {
        unsigned byX7 = the_firmware->controllerUsedRAM("x7");
        if (((Patch *)this)->needsX7())
            breakdown.append(TR("%1 bytes are used by the X7.").arg(niceBytes(byX7)));
        else
            breakdown.append(TR("%1 bytes could be used by a potential X7.").arg(niceBytes(byX7)));
        memory += byX7;
    }

    // Every unique constant needs 4 bytes
    unsigned numConstants = ((Patch *)this)->countUniqueConstants();
    memory += numConstants * 4;
    breakdown.append(TR("%1 bytes are used by %2 unique constants.")
                         .arg(niceBytes(numConstants * 4)).arg(numConstants));

    // Every (unique) patch cable needs 8 bytes
    unsigned numCables = ((Patch *)this)->countUniqueCables();
    memory += numCables * 8;
    breakdown.append(TR("%1 bytes are used by %2 unique cables.")
                         .arg(niceBytes(numCables * 8)).arg(numCables));

    // Count memory needed by the parameter values. This is a side product
    // of the deduplication happending at toCompressed
    unsigned jacktableSize;
    unsigned savedBytes;
    toDeployString(&jacktableSize, &savedBytes);
    unsigned byParameters = jacktableSize - the_firmware->initialJacktableSize();
    memory += byParameters;
    breakdown.append(TR("%1 bytes are used by parameter values.").arg(niceBytes(byParameters)));
    if (savedBytes)
        breakdown.append(TR("%1 bytes are saved by sharing duplicate input values").arg(niceBytes(savedBytes)));
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

QString Patch::canonizeNumber(double number) const
{
    // We need some rounding in order to avoid duplicating constants
    // just because of internal rounding errors.
    return QString::number(number, 'f', 10);
}
unsigned Patch::countUniqueConstants()
{
    // This algo is quite complex. Please have a look at the Droid firmware
    // thedroid.cc for the mirrored code that is the basis for this.
    QSet<QString> constants;
    constants.insert(canonizeNumber(0.0));
    constants.insert(canonizeNumber(1.0));
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isNumber()) {
            AtomNumber *an = (AtomNumber *)atom;
            double number = an->getNumber();

            if (number == 0.0)
                continue;
            constants.insert(canonizeNumber(-number));
            constants.insert(canonizeNumber(number));
            if (an->isFraction()) {
                constants.insert(canonizeNumber(1.0 / -number));
                constants.insert(canonizeNumber(1.0 / number));
            }
        }
    }
    // The memory consumption of the constants in the Droid is aligned
    // to 8, so an odd number of constants need to be rounded up.
    if (constants.count() % 2 == 0)
        return constants.count() + 1;
    else
        return constants.count();
}
unsigned int Patch::countEncoders() const
{
    unsigned count = 0;
    for (auto &controller: controllers) {
        if (controller == "e4")
            count += 4;
        else if (controller == "db8e")
            count += 1;
    }
    return count;
}
unsigned int Patch::countFaders() const
{
    unsigned count = 0;
    for (auto &controller: controllers) {
        if (controller == "m4")
            count += 4;
    }
    return count;
}

unsigned Patch::highestGatePrefix()
{
    unsigned highest_g8 = 0;
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isRegister()) {
            AtomRegister *reg = (AtomRegister *)atom;
            if (reg->getRegisterType() == REGISTER_GATE &&
                reg->getNumber() < 9 /* don't confuse with X7 */)
            {
                highest_g8 = qMax(highest_g8, reg->getG8Number());
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

    // Some circuits need MIDI. This can be provided by
    // the X7 or by a master18/36
    if (typeOfMaster() == 16) {
        for (auto section: sections)
            if (section->needsMIDI())
                return true;
    }

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

    if (!labels.isEmpty())
        s += "# LABELS: " + labelsToString() + "\n";

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

QString Patch::toBareString() const
{
    QString s;
    for (qsizetype i=0; i<controllers.length(); i++)
        s += "[" + controllers[i] + "]\n";
    if (controllers.length())
        s += "\n";
    for (qsizetype i=0; i<sections.length(); i++) {
        s += sections[i]->toBareString();
    }
    if (s.endsWith("\n\n"))
        s.chop(1);

    return s;

}
QString Patch::toDeployString(unsigned *jacktableSize, unsigned *savedBytes) const
{
    QSettings settings;
    bool renameCables = settings.value("compression/rename_cables", false).toBool();
    bool useShortnames = settings.value("compression/use_shortnames", false).toBool();
    bool deduplicateJacks = settings.value("compression/deduplicate_jacks", false).toBool();
    JackDeduplicator jdd(deduplicateJacks, useShortnames);

    const Patch *patch;
    Patch *clonedPatch = 0;

    if (renameCables) {
         clonedPatch = clone();
         clonedPatch->compressCables();
         patch = clonedPatch;
    }
    else
        patch = this;


    QString s;
    for (qsizetype i=0; i<patch->controllers.length(); i++)
        s += "[" + patch->controllers[i] + "]\n";
    for (qsizetype i=0; i<patch->sections.length(); i++)
        s += patch->sections[i]->toDeployString(jdd);

    if (clonedPatch)
        delete clonedPatch;

    if (jacktableSize)
        *jacktableSize = jdd.jacktableSize();
    if (savedBytes)
        *savedBytes = jdd.saved();
    return s;
}
bool Patch::saveContentsToFile(const QString filePath, const QString &contents) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
          return false;

    QTextStream stream(&file);
    stream << contents;
    stream.flush();
    file.close();
    if (stream.status() != QTextStream::Ok)
        return false; // failed to save

    QFile testFile(filePath);
    if (!testFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(
            0,
            TR("File cannot be opened"),
            TR("Saving the patch to the file\n\n"
               "%1\n\n"
               "seems to have succeeded, but now I cannot open the file "
               "for test reading! Please check if the file has been saved "
               "correctly or maybe choose another storage destination "
               "for your patch. Otherwise you might loose your changes.").arg(filePath),
            QMessageBox::Ok);
        return false;
    }

    QTextStream testStream(&testFile);
    QString testContents = testStream.readAll();
    testFile.close();

    if (testContents != contents) {
        QMessageBox::critical(
            0,
            TR("File content modified after saving"),
            TR("Saving the patch to the file\n\n"
               "%1\n\n"
               "seems to have succeeded, but now when I read the contents "
               "of the file they do not match the data I have written to "
               "it. It seems like the file was not saved correctly.").arg(filePath),
            QMessageBox::Ok);
        return false;
    }

    return true;
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
