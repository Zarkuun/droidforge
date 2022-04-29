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
    QString getSelectedCircuit() const;
    void accept();

private:
    void addCategoryTab(QString category, QString title);

private slots:
    void nextCategory();
    void previousCategory();
};

#endif // CIRCUITCHOOSEDIALOG_H
