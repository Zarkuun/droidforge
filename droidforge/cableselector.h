#ifndef CABLESELECTOR_H
#define CABLESELECTOR_H

#include "atomcable.h"
#include "patch.h"

#include <QGroupBox>
#include <QObject>
#include <QComboBox>

class CableSelector : public QGroupBox
{
    Q_OBJECT
    const Patch *patch;
    QComboBox *comboBox;
    QString cable;

public:
    CableSelector(QWidget *parent = nullptr);
    void setAtom(const Patch *patch, AtomCable *ac);
    void clearAtom();
    AtomCable *getAtom();
    void getFocus();

private slots:
    void cableEdited(QString text);
};

#endif // CABLESELECTOR_H
