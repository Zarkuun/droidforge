#ifndef JACKCHOOSEDIALOG_H
#define JACKCHOOSEDIALOG_H

#include "jackselector.h"
#include "jackassignment.h"

#include <QDialog>
#include <QDialogButtonBox>

class JackChooseDialog : public QDialog
{
    Q_OBJECT

    QDialogButtonBox *buttonBox;
    JackSelector *jackSelector;
    QLineEdit *lineEditSearch;

public:
    JackChooseDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    static QString chooseJack(
            const QString &circuit,
            const QStringList &used,
            jacktype_t jackType = JACKTYPE_DONT_CARE);

public slots:
    void cursorMoved(bool onActive);

private:
    void setCircuit(const QString &circuit, const QStringList &usedJacks, jacktype_t jackType);
    QString getSelectedJack() const;
};

#endif // JACKCHOOSEDIALOG_H
