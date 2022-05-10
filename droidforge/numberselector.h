#ifndef NUMBERSELECTOR_H
#define NUMBERSELECTOR_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "atomnumber.h"


class NumberSelector : public QWidget
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
    void setAtom(AtomNumber *an);
    void clearAtom();
    AtomNumber *getAtom();
    void setNumberType(atom_number_t t);

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
