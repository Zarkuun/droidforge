#include "jackchoosedialog.h"
#include "colorscheme.h"
#include "globals.h"
#include "jackselector.h"
#include "droidfirmware.h"

#include <QGridLayout>
#include <QLabel>
#include <QKeyEvent>

JackChooseDialog::JackChooseDialog(QWidget *parent)
    : Dialog{"jackchooser", parent}
{
    setWindowTitle(tr("Add new parameter"));

    // Canvas with circuit diagram
    jackSelector = new JackSelector(this);

    // Description
    labelDescription = new QLabel(this);
    labelDescription->setWordWrap(true);
    labelDescription->setMinimumWidth(JSEL_DESCRIPTION_WIDTH);
    labelDescription->setMaximumWidth(JSEL_DESCRIPTION_WIDTH);
    labelDescription->setTextFormat(Qt::RichText);
    labelDescription->setAlignment(Qt::AlignTop);
    labelDescription->setStyleSheet(
       QString("QLabel { padding: 10px; background-color : #%1; color: #%2; }")
                .arg(COLOR(JSEL_COLOR_DESCRIPTION_BACKGROUND).name(),
                     COLOR(JSEL_COLOR_DESCRIPTION).name()));

    labelJackType = new QLabel(this);
    labelJackType->setWordWrap(true);
    labelJackType->setMinimumWidth(JSEL_DESCRIPTION_WIDTH);
    labelJackType->setMaximumWidth(JSEL_DESCRIPTION_WIDTH);
    labelJackType->setTextFormat(Qt::RichText);
    labelJackType->setAlignment(Qt::AlignBottom);
    labelJackType->setStyleSheet(
       QString("QLabel { padding: 10px; background-color : #%1; color: #%2; }")
                .arg(COLOR(JSEL_COLOR_DESCRIPTION_BACKGROUND).name(),
                     COLOR(JSEL_COLOR_DESCRIPTION).name()));

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Search
    QLabel *searchLabel = new QLabel(tr("Search:"), this);
    lineEditSearch = new KeyCaptureLineEdit(this);
    connect(lineEditSearch, &QLineEdit::textChanged, jackSelector, &JackSelector::searchChanged);
    connect(lineEditSearch, &KeyCaptureLineEdit::keyPressed, jackSelector, &JackSelector::keyPressed);

    // Layout
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(jackSelector, 0, 0, 2, 2);
    mainLayout->addWidget(labelDescription, 0, 2);
    mainLayout->addWidget(labelJackType, 1, 2);
    mainLayout->addWidget(searchLabel, 2, 0);
    mainLayout->addWidget(lineEditSearch, 2, 1);
    mainLayout->addWidget(buttonBox, 2, 2);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 0);
    setLayout(mainLayout);

    connect(jackSelector, &JackSelector::cursorMoved, this, &JackChooseDialog::cursorMoved);
    connect(jackSelector, &JackSelector::accepted, this, &JackChooseDialog::accept);
}
void JackChooseDialog::setCircuit(const QString &circ, const QString &current, const QStringList &usedJacks, jacktype_t jackType)
{
    circuit = circ;
    jackSelector->setCircuit(circuit, current, usedJacks, jackType, lineEditSearch->text());
}
QString JackChooseDialog::getSelectedJack() const
{
    return jackSelector->getSelectedJack();
}
void JackChooseDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
        lineEditSearch->insert(event->text());
    else if (event->key() == Qt::Key_Backspace)
        lineEditSearch->backspace();
    else
        QDialog::keyPressEvent(event);
}
void JackChooseDialog::showEvent(QShowEvent *)
{
    lineEditSearch->selectAll();
    lineEditSearch->setFocus();
}
QString JackChooseDialog::chooseJack(const QString &circuit, const QString &current, const QStringList &used, jacktype_t jackType)
{
    static JackChooseDialog *dialog = 0;
    if (!dialog)
        dialog = new JackChooseDialog();

    dialog->setCircuit(circuit, current, used, jackType);
    if (dialog->exec() == QDialog::Accepted)
        return dialog->getSelectedJack();
    else
        return "";
}
void JackChooseDialog::cursorMoved(QString jack, jacktype_t jacktype, bool onActive)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(onActive);
    QString whence = jacktype == JACKTYPE_INPUT ? "inputs" : "outputs";
    labelDescription->setText(the_firmware->jackDescriptionHTML(circuit, whence, jack));
    labelJackType->setText(the_firmware->jackTypeDescriptionHTML(circuit, whence, jack));
}
