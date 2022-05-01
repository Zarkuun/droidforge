#ifndef JACKCHOOSEDIALOG_H
#define JACKCHOOSEDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

class JackChooseDialog : public QDialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;

public:
    JackChooseDialog(QWidget *parent = nullptr);
    ~JackChooseDialog();
    void setCircuit(QString name);
};

#endif // JACKCHOOSEDIALOG_H
