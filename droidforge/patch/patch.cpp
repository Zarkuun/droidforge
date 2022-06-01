#include "patch.h"
#include "atomcable.h"
#include "droidfirmware.h"
#include "modulebuilder.h"
#include "registercomments.h"

#include <QFileInfo>

// TODO: use index and number correctly
// Controller number goes from 1 ... 16
// Indices start at 0

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
    fileName = "";
    title = "";
    description.clear();
    libraryMetaData = "";
    registerComments.clear();
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
    otherPatch->registerComments = registerComments;
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
            QString title = snippet->getTitle();
            if (title.isEmpty()) {
                QFileInfo fi(snippet->getFilePath());
                title = fi.baseName();
            }
            clonedSection->setTitle(title);
        }
        insertSection(index++, clonedSection);
    }
    sectionIndex = startIndex; // move user directly to new section

    // TODO: If the section has no native name and it's a single
    //       section, use the patch title instead
    // TODO: Move used registers to free things if required
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
}

void Patch::removeController(int index)
{
    for (auto section: sections)
        section->shiftControllerNumbers(index + 1, -1);
    controllers.remove(index);
}

void Patch::addDescriptionLine(const QString &line)
{
    description.append(line);
}

void Patch::setDescription(const QString &d)
{
    description = d.split('\n');
    if (d.endsWith("\n"))
        description.removeLast();
}

void Patch::addRegisterComment(QChar registerName, unsigned controller, unsigned number, const QString &shorthand, const QString &comment)
{
    AtomRegister atom(registerName, controller, number);
    RegisterComment rc{atom, shorthand, comment};
    registerComments.append(rc);
}

const QString &Patch::getTitle() const
{
    return title;
}

QString Patch::getDescription() const
{
    if (description.empty())
        return "";
    else
        return description.join("\n") + "\n";
}

const Circuit *Patch::currentCircuit() const
{
    return currentSection()->currentCircuit();
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

bool Patch::needG8() const
{
    for (qsizetype i=0; i<sections.length(); i++)
        if (sections[i]->needG8())
            return true;

    return false;
}

bool Patch::needX7() const
{
    for (qsizetype i=0; i<sections.length(); i++)
        if (sections[i]->needX7())
            return true;

    return false;
}

void Patch::collectUsedRegisterAtoms(RegisterList &sl) const
{
    for (auto section: sections)
        section->collectRegisterAtoms(sl);
}

void Patch::collectAvailableRegisterAtoms(RegisterList &rl) const
{
    // TODO: Hardcode G8 and X7 here?
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

    int sectionNr=0;
    for (auto section: sections) {
        auto sectionProblems = section->collectProblems(this);
        for (auto problem: sectionProblems)
            problem->setSection(sectionNr);
        problems += sectionProblems;
        sectionNr++;
    }
}

QString Patch::problemAt(int section, const CursorPosition &pos)
{
    for (auto problem: problems) {
        if (problem->isAt(section, pos))
            return problem->getReason();
    }
    return "";
}

bool Patch::registerAvailable(AtomRegister ar) const
{
    char regType = ar.getRegisterType().toLatin1();
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

    return ar.number() >= 1 && ar.number() <= max;
}

void Patch::remapRegister(AtomRegister from, AtomRegister to)
{
    for (auto section: sections)
        section->remapRegister(from, to);
}

void Patch::removeRegisterReferences(RegisterList &rl, int ih, int oh)
{
    for (auto section: sections)
        section->removeRegisterReferences(rl, ih, oh);
}

void Patch::swapControllerNumbers(int fromNumber, int toNumber)
{
    for (auto section: sections)
        section->swapControllerNumbers(fromNumber, toNumber);
}

void Patch::shiftControllerNumbers(int number, int by)
{
    for (auto section: sections)
        section->shiftControllerNumbers(number, by);

}

QString Patch::toString() const
{
    QString s;
    if (title.isEmpty())
        s += "# Untitled patch\n";
    else
        s += "# " + title + "\n";

    if (!libraryMetaData.isEmpty())
        s += "# LIBRARY: " + libraryMetaData;

    s += "\n";

    if (!description.isEmpty()) {
        if (!s.isEmpty())
            s += "\n";
        for (qsizetype i=0; i<description.length(); i++) {
            s += "# " + description[i] + "\n";
        }
        s += "\n";
    }

    s += registerComments.toString();

    for (qsizetype i=0; i<controllers.length(); i++)
        s += "[" + controllers[i] + "]\n";

    if (!s.isEmpty())
        s += "\n";

    for (qsizetype i=0; i<sections.length(); i++)
        s += sections[i]->toString();

    while (s.endsWith("\n\n"))
        s.chop(1);
    return s;

}

bool Patch::saveToFile(const QString filePath) const
{
    QFile file(filePath);
    // TODO: filename
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
        for (nCircuit = 0; nCircuit < section->circuits.size(); nCircuit++)
        {
            circuit = section->circuits[nCircuit];
            for (nJack=0; nJack<circuit->numJackAssignments(); nJack++)
            {
                jackAssignment = circuit->jackAssignment(nJack);
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
        if (nAtom > 3) {
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
        if (nCircuit+1 < section->circuits.size()) {
            nCircuit ++;
            circuit = section->circuits[nCircuit];
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
