#include "colorscheme.h"

ColorScheme *the_colorscheme = 0;

#include <QGridLayout>
#include <QListWidget>
#include <QSettings>
#include <QColorDialog>

ColorScheme::ColorScheme(QWidget *parent)
    : Dialog{"colorscheme", parent}
{
    if (the_colorscheme == 0)
        the_colorscheme = this;

    setFocusPolicy(Qt::NoFocus);

    setWindowTitle(tr("Color scheme"));
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    QListWidget *list = new QListWidget(this);
    mainLayout->addWidget(list, 0, 0);
    QListWidgetItem *item;
    item = new QListWidgetItem(tr("Lines in general?"), list);
    item->setData(1, COLOR_LINE);

    item = new QListWidgetItem(tr("Circuits: lines"), list);
    item->setData(1, CIRV_COLOR_LINE);

    item = new QListWidgetItem(tr("Circuits: background"), list);
    item->setData(1, CIRV_COLOR_BACKGROUND);

    item = new QListWidgetItem(tr("Normal cursor"), list);
    item->setData(1, COLOR_CURSOR_NORMAL);

    item = new QListWidgetItem(tr("Problem cursor"), list);
    item->setData(1, COLOR_CURSOR_PROBLEM);

    item = new QListWidgetItem(tr("Patching cursor"), list);
    item->setData(1, COLOR_CURSOR_PATCHING);

    item = new QListWidgetItem(tr("Disabled cursor"), list);
    item->setData(1, COLOR_CURSOR_DISABLED);

    item = new QListWidgetItem(tr("Inactive cursor"), list);
    item->setData(1, COLOR_CURSOR_INACTIVE);

    item = new QListWidgetItem(tr("Input jacks"), list);
    item->setData(1, COLOR_JACK_INPUT);

    item = new QListWidgetItem(tr("Output jacks"), list);
    item->setData(1, COLOR_JACK_OUTPUT);

    item = new QListWidgetItem(tr("Rackview: register marker BG"), list);
    item->setData(1, RACV_REGMARKER_BACKGROUND);

    item = new QListWidgetItem(tr("Rackview: register marker border"), list);
    item->setData(1, RACV_REGMARKER_PEN_COLOR);

    item = new QListWidgetItem(tr("Rackview: register hilites BG"), list);
    item->setData(1, RACV_REGHILITES_BACKGROUND);

    item = new QListWidgetItem(tr("Rackview: register hilites border"), list);
    item->setData(1, RACV_REGHILITES_PEN_COLOR);

    colorDialog = new QColorDialog(this);
    colorDialog->setOption(QColorDialog::ShowAlphaChannel);
    colorDialog->setOption(QColorDialog::NoButtons);
    colorDialog->resize(200, 600);

    connect(list, &QListWidget::itemClicked, this, &ColorScheme::itemSelected);
    connect(colorDialog, &QColorDialog::currentColorChanged, this, &ColorScheme::colorChanged);
}

QColor ColorScheme::color(int index)
{
    QString key = "color/" + QString::number(index);
    if (settings.value(key).isValid())
        return settings.value(key).value<QColor>();
    else
        return QColor(128, 128, 128);
}

void ColorScheme::setColor(int index, const QColor &color)
{
    QString key = "color/" + QString::number(index);
    settings.setValue(key, color);
}

void ColorScheme::itemSelected(QListWidgetItem *item)
{
    currentIndex = item->data(1).toInt();
    colorDialog->setCurrentColor(color(currentIndex));
    colorDialog->show();
}

void ColorScheme::colorChanged(const QColor &color)
{
    setColor(currentIndex, color);
    emit changed();
}
