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
    register_type_t registerType;
    unsigned registerNumber;
    unsigned controllerNumber;
    register_type_t defaultRegisterType;
    QString allowedRegisters;

    QLineEdit *lineEditController;
    QLineEdit *lineEditNumber;
    QVBoxLayout *mainLayout;
    QHBoxLayout *valueBox;
    QList<QPushButton *>buttons;
    QLabel *labelRegister;

public:
    RegisterSelector(bool isControl, register_type_t reg, QString regs, QWidget *parent = nullptr);
    void getFocus();
    void setAtom(const Patch *, const Atom *atom);
    void clearAtom();
    Atom *getAtom() const;
    void installFocusFilter(QWidget *);

protected:
    void addRegisterButton(register_type_t reg, QString label);
    void setRegisterType(register_type_t reg);
    void switchRegister(register_type_t c);

private:
    void setControllerNumber(unsigned controller);
    void setRegisterNumber(unsigned number);
    void stripExtraChars(QLineEdit *edit);
    bool isValidRegister(register_type_t c);
    QValidator *validator() const;

private slots:
    void lineNumberEdited(QString text);
    void lineControllerEdited(QString text);

};

#endif // REGISTERSELECTOR_H
