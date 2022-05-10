#ifndef INPUTOUTPUTSELECTOR_H
#define INPUTOUTPUTSELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QList>
#include <QVBoxLayout>
#include <QGroupBox>

class AtomRegister;


class InputOutputSelector : public QGroupBox
{
    Q_OBJECT

    QLabel *labelRegister;
    QLineEdit *lineEdit;
    QList<QPushButton *>buttons;
    QVBoxLayout *mainLayout;

    QChar registerType;
    unsigned number;

public:
    explicit InputOutputSelector(QWidget *parent = nullptr);
    void setAtom(const AtomRegister *areg);
    void clearAtom();
    AtomRegister *getAtom();
    void getFocus();

private:
    void addRegisterButton(QChar reg, QString label);
    void setRegisterType(QChar reg);
    void switchRegister(QChar c);
    void setNumber(unsigned number);

signals:

private slots:
    void lineEdited(QString text);

};

#endif // INPUTOUTPUTSELECTOR_H
