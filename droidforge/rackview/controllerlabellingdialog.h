#ifndef CONTROLLERLABELLINGDIALOG_H
#define CONTROLLERLABELLINGDIALOG_H

#include "dialog.h"
#include "module.h"
#include "registerlabels.h"
#include "registerlabelwidget.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>

class ControllerLabellingDialog : public Dialog
{
    RegisterLabels &labels;
    QString controllerType;
    unsigned controllerNumber; // e.g. 2 for B2.4
    int currentRow;
    unsigned numColumns;
    QHBoxLayout *mainLayout;
    QGridLayout *gridLayout;
    QList<RegisterLabelWidget *> labelWidgets;

public:
    ControllerLabellingDialog(RegisterLabels &labels, const QPixmap *faceplate, QString controller, unsigned controllerNumber, AtomRegister jumpTo, QWidget *parent = nullptr);
    void accept();

private:
    void populate();
    void populateRegisters(Module *module, char regType, const QString &title);

private slots:
    void clear();
};

#endif // CONTROLLERLABELLINGDIALOG_H
