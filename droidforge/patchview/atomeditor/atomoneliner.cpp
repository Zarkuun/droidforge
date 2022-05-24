#include "atom.h"
#include "atominvalid.h"
#include "atomoneliner.h"
#include "jackassignment.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"
#include "tuning.h"

#include <QGridLayout>
#include <QLineEdit>


AtomOneliner::AtomOneliner(QWidget *parent)
    : Dialog("atomonliner", parent)
{
    setWindowTitle(tr("Edit value"));
    lineEdit = new QLineEdit(this);
    connect(lineEdit, &QLineEdit::returnPressed, this, &AtomOneliner::returnPressed);
    connect(lineEdit, &QLineEdit::selectionChanged, this, &AtomOneliner::selectionChanged);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(lineEdit, 0, 0);
    setLayout(mainLayout);
}

void AtomOneliner::returnPressed()
{
    Atom *atom = getAtom();
    if (atom)
        accept();
}


Atom *AtomOneliner::getAtom()
{
    QString text = lineEdit->text();
    if (jacktype == JACKTYPE_INPUT)
        return JackAssignmentInput::parseInputAtom(text);
    else if (jacktype == JACKTYPE_OUTPUT)
        return JackAssignmentOutput::parseOutputAtom(text);
    else
        return new AtomInvalid(text);
}

void AtomOneliner::selectionChanged()
{
    if (initialDeselect) {
        lineEdit->deselect();
        initialDeselect = false;
    }
}

Atom *AtomOneliner::editAtom(QPoint dialogPos, const Patch *, jacktype_t jacktype, QString start)
{
    static AtomOneliner *dialog = 0;
    if (!dialog)
        dialog = new AtomOneliner();

    if (dialog->edit(dialogPos, jacktype, start))
        return dialog->getAtom();
    else
        return 0;
}

bool AtomOneliner::edit(QPoint dialogPos, jacktype_t jt, QString start)
{
    jacktype = jt;

    QPoint offset = QPoint(ASON_X_OFFSET, ASON_Y_OFFSET);
    move(dialogPos - offset);
    initialDeselect = true;
    lineEdit->setText(start);
    lineEdit->deselect();
    return exec() == QDialog::Accepted;
}
