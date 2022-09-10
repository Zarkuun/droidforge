#ifndef ATOMSELECTOR_H
#define ATOMSELECTOR_H

#include "atom.h"
#include "atomsubselector.h"
#include "numberselector.h"
#include "patch.h"

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QScrollArea>


class AtomSelector : public QWidget
{
    Q_OBJECT
    AtomSubSelector *currentSelector;

    NumberSelector *numberSelector;
    QList<AtomSubSelector *> subSelectors;
    QList<QPushButton *> buttons;
    QLabel *labelDescription;
    QLabel *labelJackType;
    QLabel *labelJackTypeIcon;
    QScrollArea *scrollArea;
    jacktype_t jackType;

public:
    explicit AtomSelector(jacktype_t, QWidget *parent = nullptr);
    void setAllowFraction(bool);
    void setCircuitAndJack(QString circuit, QString jack);
    void setPatch(const Patch *patch);
    void setAtom(const Patch *patch, const Atom *atom);
    Atom *getAtom();
    void mousePressEvent(QMouseEvent *event);

public slots:
    void subselectorSelected(AtomSubSelector *ass);

private slots:
    void switchToSelector(int index);
    void commit();

signals:
    void comitted();

};

#endif // ATOMSELECTOR_H
