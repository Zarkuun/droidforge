#ifndef PATCHINTEGRATIONDIALOG_H
#define PATCHINTEGRATIONDIALOG_H

#include "atomregister.h"

#include <QObject>
#include <QCheckBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QDialog>

class PatchIntegrationDialog : public QDialog
{
    QLabel *labelControllers;
    QCheckBox *checkboxUseControllers;

    QLabel *labelRemap;
    QCheckBox *checkboxRemap;

    QDialogButtonBox *buttonBox;

    int needRemapJacks;
    int needRemapControls;
    int needRemapDespiteControllers;

public:
    PatchIntegrationDialog(QWidget *parent = nullptr);
    void setControllers(const QStringList &controllers);
    void setRemaps(int jacks, int controls, int controlsDespite);

public slots:
    void updateCheckboxes();
};

#endif // PATCHINTEGRATIONDIALOG_H
