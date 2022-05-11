#ifndef JACKCHOOSEDIALOG_H
#define JACKCHOOSEDIALOG_H

#include "jackselector.h"

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
    static QString chooseJack(const QString &circuit, const QStringList &used);

public slots:
    void cursorMoved(bool onActive);

private:
    void setCircuit(const QString &circuit, const QStringList &usedJacks);
    QString getSelectedJack() const;
};

#endif // JACKCHOOSEDIALOG_H
