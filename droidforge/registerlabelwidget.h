#ifndef REGISTERLABELWIDGET_H
#define REGISTERLABELWIDGET_H

#include "atomregister.h"
#include "registerlabels.h"

#include <QGroupBox>
#include <QLineEdit>

class RegisterLabelWidget : public QGroupBox
{
    AtomRegister atom;
    QLineEdit *lineEditShort;
    QLineEdit *lineEditDescription;

public:
    RegisterLabelWidget(
            AtomRegister atom,
            const QString shortLabel,
            const QString longLabel,
            QWidget *parent);
    AtomRegister getAtom() const { return atom; };
    void updateLabels(RegisterLabels &labels);
    void select();
    void clear();
};

#endif // REGISTERLABELWIDGET_H
