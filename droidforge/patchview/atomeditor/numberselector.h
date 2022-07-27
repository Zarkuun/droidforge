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
    QLabel *labelFraction;
    QPushButton *buttonTable;
    QPushButton *buttonFraction;
    QPushButton *buttonNumber;
    QPushButton *buttonVoltage;
    QPushButton *buttonPercentage;
    QPushButton *buttonOnOff;
    float number; // 1V is 1, not 0.1 here!
    atom_number_t numberType;

    QString circuit;
    QString jack;

public:
    explicit NumberSelector(QWidget *parent = nullptr);
    QString title() const { return tr("Fixed value"); };
    bool handlesAtom(const Atom *atom) const;
    void setAtom(const Patch *patch, const Atom *atom);
    void setAllowFraction(bool af);
    void setCircuitAndJack(QString c, QString j);
    void clearAtom();
    Atom *getAtom() const;
    void setNumberType(atom_number_t t);
    void getFocus();
    void installFocusFilter(QWidget *);

private:
    QString niceNumber(float v);

signals:

private slots:
    void lineEdited(QString text);
    void switchToNumber();
    void switchToVoltage();
    void switchToPercentage();
    void switchToOnOff();
    void switchToFraction();
    void openTable();
};

#endif // NUMBERSELECTOR_H
