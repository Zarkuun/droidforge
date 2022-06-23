#include "patchintegrationdialog.h"
#include "hintdialog.h"
#include "tuning.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>

PatchIntegrationDialog::PatchIntegrationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Integrate patch"));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    setLayout(mainLayout);

    HintDialog::hint("foo", tr("The patch that you are going to integrate contains controller definitions. If you add them to your patch\n"
                    "(and actually have those controllers) I make sure that all controller numbers are change accordingly so that\n"
                    "the integrated patch will work."));

    labelControllers = new QLabel();
    checkboxUseControllers = new QCheckBox(tr("Integrate these controller definitions"));
    mainLayout->addWidget(labelControllers);
    mainLayout->addWidget(checkboxUseControllers);
    connect(checkboxUseControllers, &QCheckBox::stateChanged, this, &PatchIntegrationDialog::updateCheckboxes);

    mainLayout->addSpacing(STANDARD_SPACING);

    labelRemap = new QLabel();
    checkboxRemap = new QCheckBox(tr("Automatically remap references to unused jacks / controls"));
    mainLayout->addWidget(labelRemap);
    mainLayout->addWidget(checkboxRemap);

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);
}

void PatchIntegrationDialog::setControllers(const QStringList &controllers)
{
    if (controllers.isEmpty())
        checkboxUseControllers->setDisabled(true);
    else
    {
        checkboxUseControllers->setDisabled(false);
        labelControllers->setText(
                    QString(tr("The integrated patch has controller definitions: %1").arg(controllers.join(' ').toUpper())));
    }
}

void PatchIntegrationDialog::setRemaps(int jacks, int controls, int controlsDespite)
{
    needRemapJacks = jacks;
    needRemapControls = controls;
    needRemapDespiteControllers = controlsDespite;

}

void PatchIntegrationDialog::updateCheckboxes()
{
    int controls;
    if (checkboxUseControllers->isChecked())
        controls = needRemapDespiteControllers;
    else
        controls = needRemapControls;

    if (needRemapJacks && controls)
        labelRemap->setText(tr("The integrated patch uses %1 controls and %2 registers on master/G8/X7\n"
                               "that are not free.").arg(controls).arg(needRemapJacks));
    else if (controls)
        labelRemap->setText(tr("The integrated patch uses %1 controls that are not free.").arg(controls));
    else
        labelRemap->setText(tr("The integrated patch uses %2 registers on master/G8/X7 that are not free.").arg(needRemapJacks));

    bool visible = needRemapJacks || controls;
    labelRemap->setVisible(visible);
    checkboxRemap->setVisible(visible);
}
