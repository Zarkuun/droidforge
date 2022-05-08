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
    ~JackChooseDialog();
    void setCircuit(const QString &circuit, const QStringList &usedJack);
    QString getSelectedJack() const;
    void keyPressEvent(QKeyEvent *event);

public slots:
    void cursorMoved(bool onActive);
};

#endif // JACKCHOOSEDIALOG_H
