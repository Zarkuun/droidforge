#ifndef USERMANUAL_H
#define USERMANUAL_H

#include "pageselector.h"

#include <QtGlobal>

#include <dialog.h>
#include <QtPdf/QPdfDocument>
#include <QtPdfWidgets/QPdfView>

class UserManual : public Dialog
{
    Q_OBJECT

    QPdfDocument document;
    QPdfView *pdfView;
    QPdfPageNavigator *pageNavigator;
    PageSelector *pageSelector;

public:
    UserManual(QWidget *parent = nullptr);
    void showCircuit(const QString &circuit);
    void showTopic(const QString &pageref);
    bool hasTopic(const QString &pageref);

protected:
    void keyPressEvent(QKeyEvent *) override;
    void wheelEvent(QWheelEvent *) override;

private:
    void jumpToPage(unsigned nr);

private slots:
    void openExternally();
};

extern UserManual *the_manual;

#endif // USERMANUAL_H
