#ifndef USERMANUAL_H
#define USERMANUAL_H

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

private:
    void jumpToPage(unsigned nr);
};

extern UserManual *the_manual;

#endif // USERMANUAL_H