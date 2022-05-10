#ifndef INPUTOUTPUTSELECTOR_H
#define INPUTOUTPUTSELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QList>
#include <QVBoxLayout>


class InputOutputSelector : public QWidget
{
    Q_OBJECT

    QLabel *labelRegister;
    QLineEdit *lineEdit;
    QList<QPushButton *>buttons;
    QVBoxLayout *mainLayout;
    char registerType;

public:
    explicit InputOutputSelector(QWidget *parent = nullptr);

private:
    void addButton(QString label);

signals:

};

#endif // INPUTOUTPUTSELECTOR_H
