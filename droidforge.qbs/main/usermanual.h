#ifndef USERMANUAL_H
#define USERMANUAL_H

#include <QtGlobal>
#include "os.h"

#include <dialog.h>
#include <QPdfDocument>
#include <QPdfView>

class UserManual : public Dialog
{
    Q_OBJECT

    QPdfDocument document;
    QPdfView *pdfView;
    QPdfPageNavigation *pageNavigation;

public:
    UserManual(QWidget *parent = nullptr);
    void showCircuit(const QString &circuit);

protected:
    void keyPressEvent(QKeyEvent *) override;

private:
    void jumpToPage(unsigned nr);

private slots:
    void openExternally();
};

extern UserManual *the_manual;

#endif // USERMANUAL_H
