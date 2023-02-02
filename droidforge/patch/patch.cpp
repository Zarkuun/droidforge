#include "patch.h"
#include "atomcable.h"
#include "droidfirmware.h"
#include "modulebuilder.h"
#include "registerlabels.h"
#include "globals.h"

#include <QFileInfo>

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
void Patch::addRegisterComment(register_type_t registerType, unsigned controller, unsigned number, const QString &shorthand, const QString &comment)
{
    AtomRegister atom(registerType, controller, number);
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
void Patch::collectUsedRegisterAtoms(RegisterList &sl) const
{
    // We could use the iterator but that has no const
    // version. Copying the whole iterator is no option.
    // Maybe there is a way to do that with templates.
    // If you feel like doing that, send me a pull request.
    for (auto section: sections)
        section->collectRegisterAtoms(sl);
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
    ModuleBuilder::allRegistersOf("master", 0, rl);
    ModuleBuilder::allRegistersOf("g8", 0, rl);
    ModuleBuilder::allRegistersOf("x7", 0, rl);
    unsigned controllerNumber = 1;

    for (auto &controller: controllers) {
        ModuleBuilder::allRegistersOf(controller, controllerNumber, rl);
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
        if (reg->getRegisterType() != REGISTER_OUTPUT &&
            reg->getRegisterType() != REGISTER_NORMALIZE)
            continue;

        const CursorPosition &pos = it.cursorPosition();
        const PatchSection *sec = section(it.sectionIndex());
        const Circuit *circuit = sec->circuit(pos.circuitNr);
        const JackAssignment *ja = circuit->jackAssignment(pos.row);
        if (!ja->isDisabled())
        {
            if (usedOutputs.contains(*reg)) {
                PatchProblem *prob = new PatchProblem(pos.row, pos.column,
                                                      TR("Duplicate usage of %1 as output").arg(reg->toString()));
                prob->setCircuit(pos.circuitNr);
                prob->setSection(it.sectionIndex());
                problems.append(prob);
            }
            else
                usedOutputs.append(*reg);
        }
    }

    // Check memory consumption of circuits
    unsigned usedMemory = 0;
    unsigned availableMemory = the_firmware->availableMemory();
    unsigned sectionIndex = 0;
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
        unsigned c = ar.controller();
        if (c > controllers.count())
            return false;
        QString name = controllers[c-1];
        max = the_firmware->numControllerRegisters(name, regType);
    }
    else
        max = the_firmware->numGlobalRegisters(regType);

    return ar.getNumber() >= 1 && ar.getNumber() <= max;
}
unsigned Patch::memoryFootprint() const
{
    unsigned memory = 0;
    for (auto section: sections)
        memory += section->memoryFootprint();

    for (const QString &controller: controllers)
        memory += the_firmware->controllerMemoryFootprint(controller);
    memory += the_firmware->controllerMemoryFootprint("x7");

    return memory;
}
bool Patch::needsG8()
{
    for (auto it = beginEnabled(); it != this->end(); ++it)
    {
        auto &atom = *it;
        if (atom->isRegister()) {
            AtomRegister *reg = (AtomRegister *)atom;
            if (reg->needsG8())
                return true;
        }
    }
    return false;
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
        sectionPart += sections[i]->toString();

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
bool Patch::saveToFile(const QString filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
          return false;
    QTextStream stream(&file);
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
