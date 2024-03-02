#include "atom.h"
#include "atominvalid.h"
#include "atomoneliner.h"
#include "jackassignment.h"
#include "jackassignmentinput.h"
#include "jackassignmentoutput.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QApplication>


AtomOneliner::AtomOneliner(QWidget *parent)
    : QDialog(parent)
    , lastKey(0)
{
    setWindowTitle(tr("Edit value"));
    lineEdit = new KeyCaptureLineEdit(this);

    connect(lineEdit, &QLineEdit::returnPressed, this, &AtomOneliner::returnPressed);
    connect(lineEdit, &QLineEdit::selectionChanged, this, &AtomOneliner::selectionChanged);
    connect(lineEdit, &QLineEdit::textChanged, this, &AtomOneliner::textChanged);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(lineEdit, 0, 0);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    setLayout(mainLayout);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowSystemMenuHint);
    connect(lineEdit, &KeyCaptureLineEdit::keyPressed, this, &AtomOneliner::keyPressed);
}
void AtomOneliner::returnPressed()
{
    Atom *atom = getAtom();
    if (atom) {
        accept();
        delete atom;
    }
}
Atom *AtomOneliner::getAtom()
{
    QString text = lineEdit->text();
    if (jacktype == JACKTYPE_INPUT) {
        Atom *a = JackAssignmentInput::parseInputAtom(text);
        if (a)
            return a;
        else
            return
                JackAssignmentInput::parseInputFraction(text);
    }

    else if (jacktype == JACKTYPE_OUTPUT)
        return JackAssignmentOutput::parseOutputAtom(text);
    else
        return new AtomInvalid(text);
}
void AtomOneliner::showEvent(QShowEvent *)
{
    setGeometry();
}
void AtomOneliner::keyPressed(int key)
{
    lastKey = key;
    accept();
}
void AtomOneliner::selectionChanged()
{
    // This hack avoids the initial "select all" feature
    // of the line edit, which will erase its content
    // as soon as the second character is typed.
    if (initialDeselect) {
        lineEdit->deselect();
        initialDeselect = false;
    }
}
Atom *AtomOneliner::editAtom(QRectF geometry, const Patch *, jacktype_t jacktype, QString start, int &lastKey)
{
    lastKey = 0;
    static AtomOneliner *dialog = 0;
    if (!dialog)
        dialog = new AtomOneliner();

    if (dialog->edit(geometry, jacktype, start)) {
        lastKey = dialog->getLastKey();
        return dialog->getAtom();
    }
    else
        return 0;
}
bool AtomOneliner::edit(QRectF geo, jacktype_t jt, QString start)
{
    lastKey = 0;
    geometry = geo;
    jacktype = jt;

    QFont font = QApplication::font();
    font.setPixelSize(geometry.height() * 10 / 16);
    lineEdit->setFont(font);

    lineEdit->setFixedHeight(geometry.height());
    lineEdit->setFixedWidth(geometry.width());
    lineEdit->setText(start);
    initialDeselect = true;
    lineEdit->deselect();
    return exec() == QDialog::Accepted;
}
void AtomOneliner::setGeometry()
{
    move(geometry.topLeft().toPoint());
    resize(geometry.width(), geometry.height());
}
void AtomOneliner::textChanged(QString text)
{
    if (text.startsWith('_')) // patch cable
        lineEdit->setSpaceHandling(SPACE_TO_UNDERSCORE);
    else
        lineEdit->setSpaceHandling(SPACE_IGNORED);
}
