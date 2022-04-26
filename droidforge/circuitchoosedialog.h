#ifndef CIRCUITCHOOSEDIALOG_H
#define CIRCUITCHOOSEDIALOG_H


#include <QDialogButtonBox>
#include <QPushButton>
#include <QDialog>
#include <QTabWidget>


class CircuitChooseDialog : public QDialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;

public:
    CircuitChooseDialog(QWidget *parent = nullptr);
    ~CircuitChooseDialog();
};

#endif // CIRCUITCHOOSEDIALOG_H
