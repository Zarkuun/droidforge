#include "controllerlabellingdialog.h"
#include "modulebuilder.h"
#include "registerlabelwidget.h"
#include "globals.h"
#include "mainwindow.h"

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>


ControllerLabellingDialog::ControllerLabellingDialog(MainWindow *mainWindow, RegisterLabels &labels, QString controllerType, unsigned controllerNumber, unsigned g8Number, AtomRegister jumpTo)
    : Dialog("controllerlabelling/" + controllerType, mainWindow)
    , moduleBuilder(mainWindow)
    , labels(labels)
    , controllerType(controllerType)
    , controllerNumber(controllerNumber)
    , g8Number(g8Number)
    , currentRow(0)
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignTop);
    setLayout(mainLayout);

    QLabel *faceplateLabel = new QLabel();
    QPixmap faceplatePixmap(":images/faceplates/" + controllerType + ".png");
    faceplateLabel->setPixmap(faceplatePixmap.scaledToHeight(450, Qt::SmoothTransformation));
    faceplateLabel->setAlignment(Qt::AlignTop);
    faceplateLabel->setStyleSheet(QString("QLabel { padding-top: 35px; }"));

    gridLayout = new QGridLayout();
    mainLayout->addWidget(faceplateLabel);
    mainLayout->addLayout(gridLayout);

    populate();

    // Buttons with OK/Cancel
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    QPushButton *clearButton = new QPushButton(tr("Clear"));
    connect(clearButton, &QPushButton::clicked, this, &ControllerLabellingDialog::clear);
    buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    gridLayout->setRowStretch(currentRow, 1000000000);
    gridLayout->addWidget(buttonBox, currentRow + 1, 0, 1, -1);

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
    Module *module = moduleBuilder.buildModule(controllerType);
    populateRegisters(module, REGISTER_INPUT, tr("Inputs"));
    populateRegisters(module, REGISTER_OUTPUT, tr("Outputs"));
    populateRegisters(module, REGISTER_GATE, tr("Gates"));
    populateRegisters(module, REGISTER_POT, tr("Potentiometers"));
    populateRegisters(module, REGISTER_ENCODER, tr("Encoders"));
    populateRegisters(module, REGISTER_BUTTON, tr("Buttons"));
    populateRegisters(module, REGISTER_SWITCH, tr("Switches"));
}
void ControllerLabellingDialog::populateRegisters(Module *module, char regType, const QString &title)
{
    unsigned count = module->numRegisters(regType);
    if (!count)
        return;

    unsigned columnOffset;
    if (module->getName() == "e4" && regType == REGISTER_BUTTON) {
        columnOffset = 1;
        currentRow = 0;
    }
    else
        columnOffset = 0;

    gridLayout->addWidget(new QLabel(title, this), currentRow, columnOffset, 1, -1);
    currentRow ++;

    if (regType == REGISTER_INPUT || regType == REGISTER_OUTPUT)
        numColumns = 4;
    else if (regType == REGISTER_GATE)
        numColumns = 2;
    else if (regType == REGISTER_POT && count > 4) // P10
        numColumns = 2;
    else if (module->getName() == "m4")
        numColumns = 4;
    else if (regType == REGISTER_POT) // P2B8, P4B2
        numColumns = 1;
    else if (regType == REGISTER_ENCODER) // E4
        numColumns = 1;
    else if (regType == REGISTER_BUTTON && count > 8) // B32
        numColumns = 4;
    else if (regType == REGISTER_BUTTON && module->getName() == "e4")
        numColumns = 1;
    else if (regType == REGISTER_BUTTON)
        numColumns = 2;
    else
        numColumns = 2;

    unsigned start = module->numberOffset(regType);
    unsigned column = 0;
    for (unsigned num=start+1; num<=start+count; num++) {
        if (column >= numColumns) {
            column = 0;
            currentRow ++;
        }
        else if ((module->getName() == "p10" || module->getName() == "s10") &&
                 num <= 3)
        {
            column = 0;
            currentRow ++;
        }

        AtomRegister atom(regType, controllerNumber, g8Number, num);
        QString shorthand;
        QString description;
        if (labels.contains(atom)) {
            const RegisterLabel &rl = labels[atom];
            shorthand = rl.shorthand;
            description = rl.description;
        }

        RegisterLabelWidget *rlw = new RegisterLabelWidget(atom, shorthand, description, this);
        labelWidgets.append(rlw);
        gridLayout->addWidget(rlw, currentRow, column + columnOffset);
        column ++;
    }
    currentRow++;
}
void ControllerLabellingDialog::clear()
{
    for (auto rlw: labelWidgets)
        rlw->clear();
}
