#ifndef JACKCHOOSEDIALOG_H
#define JACKCHOOSEDIALOG_H

#include "jackselector.h"

#include <QDialog>
#include <QDialogButtonBox>

class JackChooseDialog : public QDialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    JackSelector *jackSelector;

public:
    JackChooseDialog(QWidget *parent = nullptr);
    ~JackChooseDialog();
    void setCircuit(const QString &circuit);
    QString getSelectedJack() const;
};

#endif // JACKCHOOSEDIALOG_H
