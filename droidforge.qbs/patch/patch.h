#ifndef PATCH_H
#define PATCH_H

#include "patchproblem.h"
#include "patchsection.h"
#include "registerlabels.h"
#include "patchproblem.h"

#include <QStringList>

class Patch
{
    // Contents tha are saved to disk
    QString title;
    QStringList description;
    QString libraryMetaData; // break into structure later
    RegisterLabels registerLabels;
    QStringList controllers;
    QList<PatchSection *> sections;

    // Part of the versioned state for user convenience
    qsizetype sectionIndex; // part of cursor position

    QList<PatchProblem *> problems; // This is just a cache

public:
    Patch();
    ~Patch();
    Patch *clone() const;
    void cloneInto(Patch *otherPatch) const;
    QString toString() const;
    QString toCleanString() const;
    QString toBare() const;
    bool saveToFile(const QString filePath) const;

    // Simple access functions
    const QString &getTitle() const { return title; }
    bool hasTitle() const { return title != ""; }
    QString getDescription() const;
    const QString &getLibraryMetaData() const { return libraryMetaData; }
    RegisterLabels &getRegisterLabels() { return registerLabels; };
    const RegisterLabels *getRegisterLabelsPointer() const { return &registerLabels; };
    RegisterLabel registerLabel(AtomRegister atom) const;
    qsizetype numControllers() const { return controllers.size(); };
    QString controller(qsizetype i) const { return controllers[i]; };
    const QStringList &allControllers() const { return controllers; };
    bool isEmpty() const { return numSections() == 0; };
    qsizetype numSections() const { return sections.size(); };
    qsizetype currentSectionIndex() const { return sectionIndex; };
    const PatchSection *currentSection() const { return sections[sectionIndex]; };
    PatchSection *section(qsizetype i) { return sections[i]; };
    const PatchSection *section(qsizetype i) const { return sections[i]; };
    const QList<PatchSection *> &getSections() const { return sections; };
    PatchSection *currentSection() { return sections[sectionIndex]; }; // never 0
    const Circuit *currentCircuit() const; // can be 0
    void moveSection(int fromIndex, int toIndex);
    const Atom *currentAtom() const; // can be 0
    void setCursorTo(int section, const CursorPosition &pos);

    // More complex analysis
    QStringList allCables() const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    bool cableExists(const QString &cable) const;
    QString freshCableName() const;
    void collectUsedRegisterAtoms(RegisterList &) const;
    bool registerUsed(AtomRegister reg);
    bool controlUsed(AtomRegister reg);
    void collectAvailableRegisterAtoms(RegisterList &) const;
    bool hasProblems() const { return problems.count() > 0; };
    unsigned numProblems() const { return problems.count(); };
    unsigned numProblemsInSection(int i) const;
    QString problemAt(int section, const CursorPosition &pos);
    const QList<PatchProblem *> &allProblems() const { return problems; };
    const PatchProblem *problem(unsigned nr) { return problems[nr]; };
    bool registerAvailable(AtomRegister ar) const;
    unsigned memoryFootprint() const;
    bool needsG8();
    bool needsX7();

    // Modifications
    void addDescriptionLine(const QString &line);
    void addDescriptionLines(const QStringList &list);
    void setTitle(const QString &newTitle);
    void setLibraryMetaData(const QString &newLibraryMetaData) { libraryMetaData = newLibraryMetaData; }
    void switchCurrentSection(qsizetype i) { sectionIndex = i; };
    void addSection(PatchSection *section);
    void insertSection(int index, PatchSection *section);
    void mergeSections(int indexa, int indexb);
    void removeSection(int index);
    void integratePatch(const Patch *snippet);
    void reorderSections(int fromindex, int toindex);
    void swapControllersSmart(int fromindex, int toindex);
    void moveControllerSmart(int fromIndex, int toIndex);
    void swapControllerNumbers(int fromindex, int toindex);
    void shiftControllerNumbers(int number, int by=-1);
    void addController(QString name) { controllers.append(name); };
    void removeController(int index);
    void setDescription(const QString &d);
    void addRegisterComment(register_type_t registerType,
            unsigned controller,
            unsigned number,
            const QString &shorthand,
            const QString &atomcomment);
    void moveRegistersToOtherControllers(int controllerIndex, RegisterList &atomsToRemap);
    void remapRegister(AtomRegister from, AtomRegister to);
    void swapRegisters(AtomRegister regA, AtomRegister regB);
    void removeRegisterReferences(RegisterList &rl);
    void renameCable(const QString &oldName, const QString &newName);

protected:
    void updateProblems();
    void clear();

private:

public:
    // Iteration of all atoms in this patch
    class iterator {
        Atom *atom; // is 0 at end of iteration, otherwise never
        Patch *patch;
        bool onlyEnabled;
        int nSection; // always points to valid section
        int nCircuit; // always points to valid circuit
        int nJack; // always points to valid jack
        int nAtom; // always points to valid atom (1, 2, or 3)
        // if atom is != 0, the following three variables
        // always point to the objects according to the n... numbers
        PatchSection *section; // always reflects nSection
        Circuit *circuit; // always reflects nCircuit
        JackAssignment *jackAssignment; // always reflects nJack

    public:
        iterator(Patch *p, bool onlyEnabled)
            : patch(p)
            , onlyEnabled(onlyEnabled) { moveToFirstAtom(); }
        iterator() : atom(0), patch(0), onlyEnabled(false) {}
        bool advance();
        bool isOutput() const { return jackAssignment->isOutput(); };
        Atom *&operator*() { return atom; }
        bool operator != (const iterator &it) { return atom != it.atom; };
        void operator ++() { advance(); };
        int sectionIndex() const { return nSection; };
        CursorPosition cursorPosition() const {
               return CursorPosition(nCircuit, nJack, nAtom); };

    private:
        void moveToFirstAtom();
        bool advanceSection();
        bool advanceCircuit();
        bool advanceJack();
    };

    auto begin() { return iterator(this, false); }
    auto beginEnabled() { return iterator(this, true); }
    auto end() { return iterator(); }
};

#endif // PATCH_H