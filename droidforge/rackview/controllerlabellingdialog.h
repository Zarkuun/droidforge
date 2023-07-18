#ifndef CONTROLLERLABELLINGDIALOG_H
#define CONTROLLERLABELLINGDIALOG_H

#include "dialog.h"
#include "module.h"
#include "modulebuilder.h"
#include "registerlabels.h"
#include "registerlabelwidget.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>

class MainWindow;

class ControllerLabellingDialog : public Dialog
{
    ModuleBuilder moduleBuilder;
    RegisterLabels &labels;
    QString controllerType;
    unsigned controllerNumber; // e.g. 2 for B2.4
    unsigned g8Number; // e.g. 3 for G3.7
    int currentRow;
    unsigned numColumns;
    QHBoxLayout *mainLayout;
    QGridLayout *gridLayout;
    QList<RegisterLabelWidget *> labelWidgets;

public:
    ControllerLabellingDialog(MainWindow *mainWindow, RegisterLabels &labels, QString controller, unsigned controllerNumber, unsigned g8Number, AtomRegister jumpTo);
    void accept();

private:
    void populate();
    void populateRegisters(Module *module, char regType, const QString &title);

private slots:
    void clear();
};

#endif // CONTROLLERLABELLINGDIALOG_H
