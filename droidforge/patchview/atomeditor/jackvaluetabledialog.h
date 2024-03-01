#ifndef JACKVALUETABLEDIALOG_H
#define JACKVALUETABLEDIALOG_H

#include <QDialog>

class JackValueTableDialog : public QDialog
{
    double selectedValue;

public:
    JackValueTableDialog(QString circuit, QString jack, QWidget *parent=nullptr);
    double getSelectedValue() const { return selectedValue; };

private:
    void valueSelected(double value);
};

#endif // JACKVALUETABLEDIALOG_H
