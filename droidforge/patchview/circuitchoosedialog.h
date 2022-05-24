#ifndef CIRCUITCHOOSEDIALOG_H
#define CIRCUITCHOOSEDIALOG_H

#include "circuitcollection.h"
#include "dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTabWidget>
#include <QLineEdit>


typedef enum {
    JACKSELECTION_FULL = 0,
    JACKSELECTION_TYPICAL = 1,
    JACKSELECTION_ESSENTIAL = 2,
    JACKSELECTION_NONE = 3,
} jackselection_t;


class CircuitChooseDialog : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QComboBox *startJacksBox;
    QLineEdit *lineEditSearch;
    CircuitCollection *searchResults;

public:
    CircuitChooseDialog(QWidget *parent = nullptr);
    QString getSelectedCircuit() const;
    jackselection_t getJackSelection() const;
    void keyPressEvent(QKeyEvent *event);
    void accept();
    static QString chooseCircuit(QString oldCircuit);

private:
    void addCategoryTab(QString category, QString title);
    void setCurrentCircuit(QString name);

private slots:
    void nextCategory();
    void previousCategory();
    void searchChanged(QString text);
    void saveSettings();
};

#endif // CIRCUITCHOOSEDIALOG_H
