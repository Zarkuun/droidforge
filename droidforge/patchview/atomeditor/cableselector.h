#ifndef CABLESELECTOR_H
#define CABLESELECTOR_H

#include "atomcable.h"
#include "patch.h"
#include "atomsubselector.h"
#include "cableselectorlineedit.h"

#include <QGroupBox>
#include <QObject>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>

class CableSelector : public AtomSubSelector
{
    Q_OBJECT

    const Patch *patch;

    QLabel *labelIcon;
    CableSelectorLineEdit *lineEdit;
    QListWidget *listWidget;
    QString cable;

public:
    CableSelector(QWidget *parent = nullptr);
    QString title() const { return tr("Internal cable"); };
    bool handlesAtom(const Atom *atom) const;
    void setPatch(const Patch *patch);
    void setAtom(const Patch *patch, const Atom *atom);
    void clearAtom();
    Atom *getAtom() const;
    void getFocus();
    void installFocusFilter(QWidget *);

private:
    void updateList();

private slots:
    void cableEdited(QString text);
    void cableSelected(int row);
    void itemDoubleClicked();
    void updateIcon();
    void lineKeyPressed(int key);

signals:
    void committed();
};

#endif // CABLESELECTOR_H
