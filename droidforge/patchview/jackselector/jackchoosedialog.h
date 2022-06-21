#ifndef JACKCHOOSEDIALOG_H
#define JACKCHOOSEDIALOG_H

#include "jackselector.h"
#include "jackassignment.h"
#include "dialog.h"

#include <QDialogButtonBox>
#include <QLabel>

class JackChooseDialog : public Dialog
{
    Q_OBJECT

    QString circuit;
    QDialogButtonBox *buttonBox;
    JackSelector *jackSelector;
    QLineEdit *lineEditSearch;
    QLabel *labelDescription;

public:
    JackChooseDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    static QString chooseJack(const QString &circuit, const QString &current,
            const QStringList &used,
            jacktype_t jackType = JACKTYPE_DONT_CARE);

public slots:
    void cursorMoved(QString jack, jacktype_t jacktype, bool onActive);

private:
    void setCircuit(const QString &circuit, const QString &current, const QStringList &usedJacks, jacktype_t jackType);
    QString getSelectedJack() const;
    QString jackTableAsString(const QMap<float, QString> &table);
};

#endif // JACKCHOOSEDIALOG_H
