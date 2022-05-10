#ifndef REGISTERSELECTOR_H
#define REGISTERSELECTOR_H

#include <QGroupBox>
#include <QObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QList>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>

#include "atomregister.h"

class RegisterSelector : public QGroupBox
{
    Q_OBJECT

    QLineEdit *lineEditNumber;
    unsigned registerNumber;

    QVBoxLayout *mainLayout;
    QHBoxLayout *valueBox;
    QList<QPushButton *>buttons;
    QChar registerType;
    QChar defaultRegisterType;
    QLabel *labelRegister;

public:
    RegisterSelector(QChar reg, QWidget *parent = nullptr);
    void getFocus();
    void setAtom(const AtomRegister *areg);
    void clearAtom();
    AtomRegister *getAtom();

protected:
    virtual bool isControl() = 0;

    void addRegisterButton(QChar reg, QString label);
    void setRegisterType(QChar reg);
    void switchRegister(QChar c);

private:
    void setRegisterNumber(unsigned number);

private slots:
    void lineEdited(QString text);

};

#endif // REGISTERSELECTOR_H
