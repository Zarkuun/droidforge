#ifndef ATOMSELECTOR_H
#define ATOMSELECTOR_H

#include "atom.h"
#include "atomsubselector.h"
#include "patch.h"

#include <QWidget>
#include <QPushButton>


class AtomSelector : public QWidget
{
    Q_OBJECT
    AtomSubSelector *currentSelector;

    QList<AtomSubSelector *> subSelectors;
    QList<QPushButton *> buttons;

public:
    explicit AtomSelector(jacktype_t, QWidget *parent = nullptr);
    void setAtom(const Patch *patch, const Atom *atom);
    Atom *getAtom();
    // void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void subselectorSelected(AtomSubSelector *ass);

private slots:
    void switchToSelector(int index);

};

#endif // ATOMSELECTOR_H
