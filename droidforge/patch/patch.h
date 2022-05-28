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

    // #define T Atom
    class iterator {
        Atom *atom;
        Patch *patch;
        int nSection;
        int nCircuit;
        int nJack;
        int nAtom;

    public:
        iterator(Patch *p) : atom(0), patch(p), nSection(0), nCircuit(0), nJack(0), nAtom(-1) {
            advance();
        }
        Atom *&operator*() { return atom; }
        Atom **operator->() { return &atom; }

    private:

        bool advanceSection() {
            nSection++;
            nCircuit = 0;
            nJack = 0;
            nAtom = 0;
            return nSection < patch->sections.size();
        }

        bool advanceCircuit() {
            nCircuit++;
            nJack = 0;
            nAtom = 0;
            PatchSection *section = patch->sections[nSection];
            while (nCircuit >= section->circuits.size()) {
                if (!advanceSection())
                    return false; // was in last section
                // nCircuit was reset to 0 by advanceSection()
                section = patch->sections[nSection];
            }
            return true;
        }

        bool advanceJack() {
            nJack++;
            nAtom = 0;
            PatchSection *section = patch->sections[nSection];
            Circuit *circuit = section->circuits[nCircuit];
            while (nJack >= circuit->numJackAssignments()) {
                if (!advanceCircuit())
                    return false;
                section = patch->sections[nSection];
                circuit = section->circuits[nCircuit];
            }
            return true;
        }

        bool advance() {
            nAtom ++;
            PatchSection *section = patch->sections[nSection];
            Circuit *circuit = section->circuits[nCircuit];
            JackAssignment *ja = circuit->jackAssignment(nJack);
            while (!ja->atomAt(nAtom+1)) {
                nAtom++;
                if (nAtom > 3) {
                    if (!advanceCircuit())
                        return false;
                }
                section = patch->sections[nSection];
                circuit = section->circuits[nCircuit];
                ja = circuit->jackAssignment(nJack);
            }
            atom = ja->atomAt(nAtom+1);
            return true;
        }
    };

    iterator begin() { return iterator(this); }
    /*
    ////
    ////     T *i = nullptr;
    //// public:
    ////     using difference_type = qsizetype;
    ////     using value_type = T;
    ////     // libstdc++ shipped with gcc < 11 does not have a fix for defect LWG 3346
#if //// __cplusplus >= 202002L && (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 11)
    ////     using iterator_category = std::contiguous_iterator_tag;
    ////     using element_type = value_type;
#els//// e
    ////     using iterator_category = std::random_access_iterator_tag;
#end//// if

    ////     using pointer = T *;
    ////     using reference = T &;
    ////     inline constexpr iterator() = default;
    ////     inline iterator(T *n) : i(n) {}
    ////     inline T &operator*() const { return *i; }
    ////     inline T *operator->() const { return i; }
    };
    */

private:
};

#endif // PATCH_H
