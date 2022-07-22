#ifndef CIRCUITCHOOSEDIALOG_H
#define CIRCUITCHOOSEDIALOG_H

#include "circuitcollection.h"
#include "dialog.h"
#include "circuit.h"
#include "keycapturelineedit.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTabWidget>
#include <QLineEdit>


class CircuitChooseDialog : public Dialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QComboBox *startJacksBox;
    KeyCaptureLineEdit *lineEditSearch;
    CircuitCollection *searchResults;

    CircuitChooseDialog(QWidget *parent = nullptr);
    QString getSelectedCircuit() const;
    jackselection_t getJackSelection() const;

public:
    static QString chooseCircuit(jackselection_t &jsel);
    static QString chooseCircuit(QString oldCircuit);

protected:
    void keyPressEvent(QKeyEvent *event);
    void showEvent(QShowEvent *event);
    void accept();

private:
    static QString chooseCircuit(jackselection_t &jsel, QString oldCircuit);
    void addCategoryTab(QString category, QString title);
    void setCurrentCircuit(QString name);
    void switchToNextCategory();
    void switchToPreviousCategory();
    void switchToFirstCategory();
    void switchToLastCategory();

private slots:
    void searchChanged(QString text);
    void saveSettings();
#ifdef HAVE_MIDI
    void showManual();
#endif
    void keyPressed(int key);
};

#endif // CIRCUITCHOOSEDIALOG_H
