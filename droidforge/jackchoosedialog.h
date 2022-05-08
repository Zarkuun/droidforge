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
    void setCircuit(const QString &circuit, const QStringList &usedJack);
    QString getSelectedJack() const;

public slots:
    void cursorMoved(bool onActive);
};

#endif // JACKCHOOSEDIALOG_H
