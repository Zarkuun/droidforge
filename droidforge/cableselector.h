#ifndef CABLESELECTOR_H
#define CABLESELECTOR_H

#include "atomcable.h"
#include "patch.h"
#include "atomsubselector.h"

#include <QGroupBox>
#include <QObject>
#include <QComboBox>

class CableSelector : public AtomSubSelector
{
    Q_OBJECT
    const Patch *patch;
    QComboBox *comboBox;
    QString cable;

public:
    CableSelector(QWidget *parent = nullptr);
    QString title() const { return tr("Internal cable"); };
    bool handlesAtom(const Atom *atom) const;
    void setAtom(const Patch *patch, const Atom *atom);
    void clearAtom();
    Atom *getAtom() const;
    void getFocus();

private slots:
    void cableEdited(QString text);
};

#endif // CABLESELECTOR_H
