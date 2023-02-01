#ifndef ATOMONELINER_H
#define ATOMONELINER_H

#include "atom.h"
#include "patch.h"
#include "keycapturelineedit.h"

#include <QPoint>
#include <QDialog>

class AtomOneliner : public QDialog
{
    Q_OBJECT

    KeyCaptureLineEdit *lineEdit;
    jacktype_t jacktype;
    bool initialDeselect;
    int lastKey;
    QRectF geometry;

    AtomOneliner(QWidget *parent = nullptr);
    void returnPressed();
    void selectionChanged();
    bool edit(QRectF geometry, jacktype_t jacktype, QString start);

public:
    static Atom *editAtom(QRectF geometry, const Patch *, jacktype_t jacktype, QString start, int &lastKey);
    Atom *getAtom();
    int getLastKey() const { return lastKey; };

protected:
    void showEvent(QShowEvent *);

private slots:
    void keyPressed(int);
    void setGeometry();
};

#endif // ATOMONELINER_H
