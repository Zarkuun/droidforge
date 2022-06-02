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
    QString fileName;
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
    bool saveToFile(const QString filePath) const;

    // Simple access functions
    const QString &getTitle() const;
    const QString &getFilePath() const { return fileName; };
    QString getDescription() const;
    const QString &getLibraryMetaData() const { return libraryMetaData; }
    RegisterLabels &getRegisterLabels() { return registerLabels; };
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
    const Atom *currentAtom() const; // can be 0
    void setCursorTo(int section, const CursorPosition &pos);

    // More complex analysis
    QStringList allCables() const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    bool needG8() const; // TODO: Do we need this?
    bool needX7() const; // TODO: Do we need this?
    void collectUsedRegisterAtoms(RegisterList &) const;
    void collectAvailableRegisterAtoms(RegisterList &) const;
    unsigned numProblems() const { return problems.count(); };
    unsigned numProblemsInSection(int i) const;
    QString problemAt(int section, const CursorPosition &pos);
    const QList<PatchProblem *> &allProblems() const { return problems; };
    const PatchProblem *problem(unsigned nr) { return problems[nr]; };
    bool registerAvailable(AtomRegister ar) const;

    // Modifications
    void addDescriptionLine(const QString &line);
    void setFilePath(const QString &f) { fileName = f; };
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
    void swapControllerNumbers(int fromindex, int toindex);
    void shiftControllerNumbers(int number, int by=-1);
    void addController(QString name) { controllers.append(name); };
    void removeController(int index);
    void setDescription(const QString &d);
    void addRegisterComment(
            QChar registerName,
            unsigned controller,
            unsigned number,
            const QString &shorthand,
            const QString &atomcomment);
    void remapRegister(AtomRegister from, AtomRegister to);
    void removeRegisterReferences(RegisterList &rl, int ih, int oh);
    void renameCable(const QString &oldName, const QString &newName);

protected:
    void updateProblems();
    void clear();

private:

public:
    // Iteration of all atoms in this patch
    // TODO: Move to own file?
    class iterator {
        Atom *atom; // is 0 at end of iteration, otherwise never
        Patch *patch;
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
        iterator(Patch *p) : patch(p) { moveToFirstAtom(); }
        iterator() : atom(0), patch(0) {}
        bool advance();
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

    auto begin() { return iterator(this); }
    auto end() { return iterator(); }

};

#endif // PATCH_H
