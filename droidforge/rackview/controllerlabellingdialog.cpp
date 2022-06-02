#include "controllerlabellingdialog.h"
#include "modulebuilder.h"
#include "registerlabelwidget.h"

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>

ControllerLabellingDialog::ControllerLabellingDialog(RegisterLabels &labels, QString controllerType, unsigned controllerNumber, AtomRegister jumpTo, QWidget *parent)
    : Dialog("controllerlabelling/" + controllerType, parent)
    , labels(labels)
    , controllerType(controllerType)
    , controllerNumber(controllerNumber)
    , currentRow(0)
{
    mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    populate();

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->setRowStretch(currentRow, 1000000000);
    mainLayout->addWidget(buttonBox, currentRow + 1, 0, 1, -1);

    for (auto rlw: labelWidgets) {
        if (rlw->getAtom() == jumpTo)
            rlw->select();
    }
}

void ControllerLabellingDialog::accept()
{
    for (auto rlw: labelWidgets)
        rlw->updateLabels(labels);
    Dialog::accept();
}

void ControllerLabellingDialog::populate()
{
    Module *module = ModuleBuilder::buildModule(controllerType);
    populateRegisters(module, REGISTER_INPUT, tr("Inputs"));
    populateRegisters(module, REGISTER_OUTPUT, tr("Outputs"));
    populateRegisters(module, REGISTER_GATE, tr("Gates"));
    populateRegisters(module, REGISTER_POT, tr("Potentiometers"));
    populateRegisters(module, REGISTER_BUTTON, tr("Buttons"));
    populateRegisters(module, REGISTER_SWITCH, tr("Switches"));
}

void ControllerLabellingDialog::populateRegisters(Module *module, char regType, const QString &title)
{
    unsigned count = module->numRegisters(regType);
    if (!count)
        return;

    mainLayout->addWidget(new QLabel(title, this), currentRow, 0, 1, -1);
    currentRow ++;

    unsigned numColumns;
    if (regType == REGISTER_INPUT || regType == REGISTER_OUTPUT)
        numColumns = 4;
    else if (regType == REGISTER_GATE)
        numColumns = 2;
    else if (regType == REGISTER_POT && count > 4) // P10
        numColumns = 2;
    else if (module->name() == "m4")
        numColumns = 4;
    else if (regType == REGISTER_POT) // P2B8, P4B2
        numColumns = 1;
    else if (regType == REGISTER_BUTTON && count > 8) // B32
        numColumns = 4;
    else if (regType == REGISTER_BUTTON)
        numColumns = 2;
    else
        numColumns = 2;

    unsigned column = 0;
    unsigned start = module->numberOffset(regType);
    for (unsigned num=start+1; num<=start+count; num++) {
        if (column >= numColumns) {
            column = 0;
            currentRow ++;
        }
        else if ((module->name() == "p10" || module->name() == "s10") &&
                 num <= 3)
        {
            column = 0;
            currentRow ++;
        }

        AtomRegister atom(regType, controllerNumber, num);
        QString shorthand;
        QString description;
        if (labels.contains(atom)) {
            const RegisterLabel &rl = labels[atom];
            shorthand = rl.shorthand;
            description = rl.description;
        }
        else {
            qDebug() << "NIX FUER" << atom.toString();
            for (auto &l: labels)
                qDebug() << "LAB:" << l.atom;
        }


        RegisterLabelWidget *rlw = new RegisterLabelWidget(atom, shorthand, description, this);
        labelWidgets.append(rlw);
        mainLayout->addWidget(rlw, currentRow, column);
        column ++;
    }
    currentRow++;
}
