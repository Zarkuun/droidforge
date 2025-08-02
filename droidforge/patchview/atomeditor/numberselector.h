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
    QLabel *labelPrefix;
    QLabel *labelUnit;
    QPushButton *buttonTable;
    QPushButton *buttonFraction;
    QPushButton *buttonNumber;
    QPushButton *buttonVoltage;
    QPushButton *buttonPercentage;
    QPushButton *buttonOnOff;
    QPushButton *buttonText;
    double number; // 1V is 1, not 0.1 here!

    bool isText;
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
    void getFocus();
    void installFocusFilter(QWidget *);

signals:

private slots:
    void lineEdited(QString text);
    void switchToNumber();
    void switchToVoltage();
    void switchToPercentage();
    void switchToOnOff();
    void switchToFraction();
    void switchToText();
    void openTable();

private:
    void guessNumberFromText();
    void setNumberType(atom_number_t t);
    void setTextType();
};

#endif // NUMBERSELECTOR_H
