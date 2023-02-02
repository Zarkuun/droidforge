#ifndef CONTROLLERCHOOSEDIALOG_H
#define CONTROLLERCHOOSEDIALOG_H

#include "controllerselector.h"
#include "dialog.h"

#include <QDialogButtonBox>

class ControllerChooseDialog : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    ControllerSelector *controllerSelector;

    ControllerChooseDialog(QWidget *parent = nullptr);

public:
    static QString chooseController();
    const QString &getSelectedController() const;

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void purchase();
};

#endif // CONTROLLERCHOOSEDIALOG_H
