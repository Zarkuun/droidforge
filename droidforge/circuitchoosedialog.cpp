#include "circuitchoosedialog.h"
#include "circuitcollection.h"


#include <QGridLayout>

CircuitChooseDialog::CircuitChooseDialog(QWidget *parent)
    : QDialog(parent)
{
    resize(500, 300);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                     this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


    tabWidget = new QTabWidget(this);
    CircuitCollection *ccModulation = new CircuitCollection(tr("Modulation"), this);
    CircuitCollection *ccSequencing = new CircuitCollection(tr("Sequencing"), this);
    tabWidget->addTab(ccModulation, ccModulation->getTitle());
    tabWidget->addTab(ccSequencing, ccSequencing->getTitle());

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Add new circuit"));
}


CircuitChooseDialog::~CircuitChooseDialog()
{

}
