#ifndef PATCH_H
#define PATCH_H

#include "patchsection.h"
#include "registercomments.h"

#include <QStringList>

class Patch
{
    QString fileName;
    QString title;
    QStringList description;
    QString libraryMetaData; // break into structure later
    RegisterComments *registerComments;
    QStringList controllers;
    QList<PatchSection *> sections;
    qsizetype sectionIndex; // part of cursor position

public:
    Patch();
    ~Patch();
    Patch *clone() const;
    qsizetype numControllers() const { return controllers.size(); };
    QString controller(qsizetype i) const { return controllers[i]; };
    const QStringList &allControllers() const { return controllers; };
    qsizetype numSections() const { return sections.size(); };
    bool isEmpty() const { return numSections() == 0; };
    qsizetype currentSectionIndex() const { return sectionIndex; };
    void setCurrentSectionIndex(qsizetype i) { sectionIndex = i; };
    PatchSection *section(qsizetype i) { return sections[i]; };
    void addSection(PatchSection *section);
    void insertSection(int index, PatchSection *section);
    void mergeSections(int indexa, int indexb);
    void removeSection(int index);
    void integratePatch(const Patch *snippet);
    void deleteSection(int index);
    void reorderSections(int fromindex, int toindex);
    void swapControllersSmart(int fromindex, int toindex);
    void swapControllerNumbers(int fromindex, int toindex);
    void shiftControllerNumbers(int number, int by=-1);
    void addController(QString name) { controllers.append(name); };
    void removeController(int index);
    bool saveToFile(QString filename);
    void addDescriptionLine(const QString &line);
    void setDescription(const QString &d);
    void addRegisterComment(
            QChar registerName,
            unsigned controller,
            unsigned number,
            const QString &shorthand,
            const QString &atomcomment);

    QString toString();
    const QString &getTitle() const;
    void setFileName(const QString &f) { fileName = f; };
    const QString &getFileName() const { return fileName; };
    QString getDescription() const;
    void setTitle(const QString &newTitle);
    const QString &getLibraryMetaData() const { return libraryMetaData; }
    void setLibraryMetaData(const QString &newLibraryMetaData) { libraryMetaData = newLibraryMetaData; }
    QStringList allCables() const;
    void findCableConnections(const QString &cable, int &asInput, int &asOutput) const;
    bool needG8() const; // TODO: Do we need this?
    bool needX7() const; // TODO: Do we need this?
    void collectUsedRegisterAtoms(RegisterList &) const;
    void collectAvailableRegisterAtoms(RegisterList &) const;
    void remapRegister(AtomRegister from, AtomRegister to);
    void removeRegisterReferences(RegisterList &rl, int ih, int oh);

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

    private:
        void moveToFirstAtom();
        bool advanceSection();
        bool advanceCircuit();
        bool advanceJack();
    };

    auto begin() { return iterator(this); }
    auto end() { return iterator(); }

private:
};

#endif // PATCH_H
