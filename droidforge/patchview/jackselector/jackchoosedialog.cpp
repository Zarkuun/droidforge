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
    labelDescription->setStyleSheet(
       QString("QLabel { padding: 10px; background-color : #%1; color: #%2; }")
                .arg(COLOR(JSEL_COLOR_DESCRIPTION_BACKGROUND).name(),
                     COLOR(JSEL_COLOR_DESCRIPTION).name()));

    // Buttons with OK/Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Search
    QLabel *searchLabel = new QLabel(tr("Search:"), this);
    lineEditSearch = new QLineEdit(this);
    connect(lineEditSearch, &QLineEdit::textChanged, jackSelector, &JackSelector::searchChanged);

    // Layout
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(jackSelector, 0, 0, 1, 2);
    mainLayout->addWidget(labelDescription, 0, 2);
    mainLayout->addWidget(searchLabel, 1, 0);
    mainLayout->addWidget(lineEditSearch, 1, 1);
    mainLayout->addWidget(buttonBox, 1, 2);
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
    QString description = the_firmware->jackDescriptionHTML(circuit, whence, jack);
    auto table = the_firmware->jackValueTable(circuit, whence, jack);
    if (!table.empty())
        description += jackTableAsString(table);
    else
        description += "<br>";

    if (jacktype == JACKTYPE_INPUT && the_firmware->jackHasDefaultvalue(circuit, whence, jack)) {
        float default_value = the_firmware->jackDefaultvalue(circuit, whence, jack);
        description += "<br>" + tr("Default value: %1").arg(default_value);
    }

    labelDescription->setText(description);
}
QString JackChooseDialog::jackTableAsString(const QMap<float, QString> &table)
{
    QString text = "<br><br>";
    for (auto it = table.keyBegin(); it != table.keyEnd(); ++it)
    {
        float value = *it;
        QString description = table[value];
        text += QString::number(value) + ": " + description + "<br>";
    }
    return text;
}
