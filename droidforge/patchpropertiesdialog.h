#ifndef PATCHPROPERTIESDIALOG_H
#define PATCHPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class PatchPropertiesDialog;
}

class PatchPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PatchPropertiesDialog(
            const QString &title,
            const QString &description,
            QWidget *parent = nullptr);
    ~PatchPropertiesDialog();

    QString getTitle();
    QString getDescription();

private:
    Ui::PatchPropertiesDialog *ui;
};

#endif // PATCHPROPERTIESDIALOG_H
