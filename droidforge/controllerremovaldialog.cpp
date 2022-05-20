#include "controllerremovaldialog.h"
#include "tuning.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

ControllerRemovalDialog::ControllerRemovalDialog(QWidget *parent)
    : Dialog("controllerremoval", parent)
{
    setWindowTitle(tr("Remove controller defintion from this patch"));
    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    mainLayout->addWidget(new QLabel(tr("The following controlls of this module are "
                                       "used by your current patch. What shall we do?")));

    labelRemap = new QLabel();
    mainLayout->addWidget(labelRemap);
    QFont font("Hack"); // TODO: more portable way for monospaced font. Or ship HACK.
    labelRemap->setFont(font);
    mainLayout->addSpacing(STANDARD_SPACING);

    // Checkbox for remapping
    checkboxRemap = new QCheckBox(tr("Try to remap these controls to other controllers as far as possible "));
    checkboxRemap->setChecked(true);
    mainLayout->addWidget(checkboxRemap);
    mainLayout->addSpacing(STANDARD_SPACING);

    // Options for handling of non-remappable inputs
    QGroupBox *inputBox = new QGroupBox(tr("Inputs (buttons, potentiometers, switches)"), this);
    QVBoxLayout *inputLayout = new QVBoxLayout;
    inputBox->setLayout(inputLayout);
    rbInputLeave = new QRadioButton(tr("Leave references to these controls broken"), inputBox);
    rbInputSetToOne = new QRadioButton(tr("Remove references from attenuation and offsets, set to 1 for main input values"), inputBox);
    rbInputRemove = new QRadioButton(tr("Remove these references"), inputBox);
    inputLayout->addWidget(rbInputSetToOne);
    rbInputLeave->setChecked(true);
    inputLayout->addWidget(rbInputLeave);
    inputLayout->addWidget(rbInputRemove);
    mainLayout->addWidget(inputBox);

    mainLayout->addSpacing(STANDARD_SPACING);

    // Options for handling of non-remappable outputs
    QGroupBox *outputBox = new QGroupBox(tr("Outputs (LEDs)"), this);
    QVBoxLayout *outputLayout = new QVBoxLayout;
    outputBox->setLayout(outputLayout);
    rbOutputLeave = new QRadioButton(tr("Leave references to these LEDs broken"), outputBox);
    rbOutputRemove = new QRadioButton(tr("Remove these references"), outputBox);
    outputLayout->addWidget(rbOutputLeave);
    outputLayout->addWidget(rbOutputRemove);
    rbOutputLeave->setChecked(true);
    mainLayout->addWidget(outputBox);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
}

void ControllerRemovalDialog::setRegistersToRemap(const RegisterList &rl)
{
    QString text;
    for (auto& entry: rl)
        text += entry.toString() + " ";
    labelRemap->setText(text.trimmed());
}

bool ControllerRemovalDialog::shouldRemap() const
{
    return checkboxRemap->isChecked();
}

ControllerRemovalDialog::InputHandling ControllerRemovalDialog::inputHandling() const
{
    if (rbInputLeave->isChecked())
        return INPUT_LEAVE;
    else if (rbInputRemove->isChecked())
        return INPUT_REMOVE;
    else
        return INPUT_SET_TO_ONE;
}

ControllerRemovalDialog::OutputHandling ControllerRemovalDialog::outputHandling() const
{
    if (rbOutputLeave->isChecked())
        return OUTPUT_LEAVE;
    else
        return OUTPUT_REMOVE;
}
