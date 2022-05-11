#ifndef NUMBERSELECTOR_H
#define NUMBERSELECTOR_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

#include "atomnumber.h"
#include "atomsubselector.h"


class NumberSelector : public AtomSubSelector
{
    Q_OBJECT

    QLineEdit *lineEdit;
    QLabel *labelUnit;
    QPushButton *buttonNumber;
    QPushButton *buttonVoltage;
    QPushButton *buttonPercentage;
    QPushButton *buttonOnOff;
    float number; // 1V is 1, not 0.1 here!
    atom_number_t numberType;

public:
    explicit NumberSelector(QWidget *parent = nullptr);
    QString title() const { return tr("Fixed number"); };
    bool handlesAtom(const Atom *atom) const;
    void setAtom(const Patch *patch, const Atom *atom);
    void clearAtom();
    Atom *getAtom() const;
    void setNumberType(atom_number_t t);
    void getFocus();

private:
    QString niceNumber(float v);

signals:

private slots:
    void lineEdited(QString text);
    void switchToNumber();
    void switchToVoltage();
    void switchToPercentage();
    void switchToOnOff();
};

#endif // NUMBERSELECTOR_H
