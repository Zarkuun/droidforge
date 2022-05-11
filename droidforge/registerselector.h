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
#include "atomsubselector.h"

class RegisterSelector : public AtomSubSelector
{
    Q_OBJECT

    bool isControl;
    QChar registerType;
    unsigned registerNumber;
    unsigned controllerNumber;
    QChar defaultRegisterType;
    QString allowedRegisters;

    QLineEdit *lineEditController;
    QLineEdit *lineEditNumber;
    QVBoxLayout *mainLayout;
    QHBoxLayout *valueBox;
    QList<QPushButton *>buttons;
    QLabel *labelRegister;

public:
    RegisterSelector(bool isControl, QChar reg, QString regs, QWidget *parent = nullptr);
    void getFocus();
    void setAtom(const Patch *, const Atom *atom);
    void clearAtom();
    Atom *getAtom() const;

protected:
    void addRegisterButton(QChar reg, QString label);
    void setRegisterType(QChar reg);
    void switchRegister(QChar c);

private:
    void setControllerNumber(unsigned controller);
    void setRegisterNumber(unsigned number);
    void stripExtraChars(QLineEdit *edit);
    bool isValidRegister(QChar c);
    QValidator *validator() const;

private slots:
    void lineNumberEdited(QString text);
    void lineControllerEdited(QString text);

};

#endif // REGISTERSELECTOR_H
