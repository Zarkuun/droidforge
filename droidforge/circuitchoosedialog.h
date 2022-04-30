#ifndef CIRCUITCHOOSEDIALOG_H
#define CIRCUITCHOOSEDIALOG_H


#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDialog>
#include <QTabWidget>


typedef enum {
    JACKSELECTION_FULL = 0,
    JACKSELECTION_TYPICAL = 1,
    JACKSELECTION_ESSENTIAL = 2,
    JACKSELECTION_NONE = 3,
} jackselection_t;


class CircuitChooseDialog : public QDialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QComboBox *startJacksBox;

public:
    CircuitChooseDialog(QWidget *parent = nullptr);
    ~CircuitChooseDialog();
    QString getSelectedCircuit() const;
    jackselection_t getJackSelection() const;
    void accept();

private:
    void addCategoryTab(QString category, QString title);

private slots:
    void nextCategory();
    void previousCategory();
};

#endif // CIRCUITCHOOSEDIALOG_H
