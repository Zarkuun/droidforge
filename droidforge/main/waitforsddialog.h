#ifndef WAITFORSDDIALOG_H
#define WAITFORSDDIALOG_H

#include "dialog.h"
#include "mainwindow.h"
#include "patchoperator.h"

class WaitForSDDialog : public Dialog
{
    Q_OBJECT
    PatchOperator *patchOperator;

public:
    WaitForSDDialog(QString title, QString text, MainWindow *mainWindow, PatchOperator *po);
    QString waitForSD();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void timedCheckSD();
};

#endif // WAITFORSDDIALOG_H
