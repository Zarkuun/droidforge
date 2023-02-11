#ifndef CONTROLLERCHOOSEDIALOG_H
#define CONTROLLERCHOOSEDIALOG_H

#include "controllerselector.h"
#include "dialog.h"

#include <QDialogButtonBox>

class MainWindow;

class ControllerChooseDialog : public Dialog
{
    Q_OBJECT

    MainWindow *mainWindow;
    QDialogButtonBox *buttonBox;
    ControllerSelector *controllerSelector;

public:
    ControllerChooseDialog(MainWindow *mainWindow);
    QString chooseController();
    const QString &getSelectedController() const;

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void purchase();
};

#endif // CONTROLLERCHOOSEDIALOG_H
