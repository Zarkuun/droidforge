#ifndef CIRCUITCHOOSEDIALOG_H
#define CIRCUITCHOOSEDIALOG_H

#include "circuitcollection.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDialog>
#include <QTabWidget>
#include <QLineEdit>


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
    QLineEdit *lineEditSearch;
    CircuitCollection *searchResults;

public:
    CircuitChooseDialog(QWidget *parent = nullptr);
    ~CircuitChooseDialog();
    QString getSelectedCircuit() const;
    jackselection_t getJackSelection() const;
    void keyPressEvent(QKeyEvent *event);
    void accept();

private:
    void addCategoryTab(QString category, QString title);

private slots:
    void nextCategory();
    void previousCategory();
    void searchChanged(QString text);
    void saveSettings();
};

#endif // CIRCUITCHOOSEDIALOG_H
