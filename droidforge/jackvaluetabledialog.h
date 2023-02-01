#ifndef JACKVALUETABLEDIALOG_H
#define JACKVALUETABLEDIALOG_H

#include <QDialog>

class JackValueTableDialog : public QDialog
{
    float selectedValue;

public:
    JackValueTableDialog(QString circuit, QString jack, QWidget *parent=nullptr);
    float getSelectedValue() const { return selectedValue; };

private:
    void valueSelected(float value);
};

#endif // JACKVALUETABLEDIALOG_H
