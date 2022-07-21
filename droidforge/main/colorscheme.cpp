#include "colorscheme.h"
#include "globals.h"
#include "tuning.h"

ColorScheme *the_colorscheme = 0;

#include <QGridLayout>
#include <QListWidget>
#include <QSettings>
#include <QColorDialog>
#include <QFile>
#include <QMap>
#include <QColor>
#include <QLabel>
#include <QApplication>
#include <QStyleFactory>

#include "colors_light.h"
#include "colors_dark.h"


ColorScheme::ColorScheme(QWidget *parent)
    : Dialog{"colorscheme", parent}
{
    if (the_colorscheme == 0)
        the_colorscheme = this;

#ifdef Q_OS_MAC
    QLabel label("am I in the dark?");
    int text_hsv_value = label.palette().color(QPalette::WindowText).value();
    int bg_hsv_value = label.palette().color(QPalette::Window).value();
    shout << "text" << text_hsv_value << "bg" << bg_hsv_value;
    dark = text_hsv_value > bg_hsv_value;
#endif

#ifdef Q_OS_WIN
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
    dark = settings.value( "AppsUseLightTheme", 1 ).toInt() == 0;
    if (dark) {
        // set style
        qApp->setStyle(QStyleFactory::create("Fusion"));
        // modify palette to dark
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window,QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
        darkPalette.setColor(QPalette::Base,QColor(42,42,42));
        darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
        darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
        darkPalette.setColor(QPalette::ToolTipText,Qt::white);
        darkPalette.setColor(QPalette::Text,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(100, 100, 100)); // Das ist im Menu, richtig
        darkPalette.setColor(QPalette::Light,QColor(50, 50, 50));
        darkPalette.setColor(QPalette::Mid,QColor(0, 255, 0));
        darkPalette.setColor(QPalette::Midlight,QColor(0, 0, 255));
        darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
        darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
        darkPalette.setColor(QPalette::Button,QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
        darkPalette.setColor(QPalette::BrightText,Qt::red);
        darkPalette.setColor(QPalette::Link,QColor(42,130,218));
        darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
        darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
        darkPalette.setColor(QPalette::HighlightedText,Qt::white);
        darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));
        qApp->setPalette(darkPalette);
    }
#endif

    background = QPixmap(dark ?
                        ":images/background_dark.png" :
                        ":images/background_light.png")
                  .scaledToHeight(BACKGROUND_PIXMAP_HEIGHT);

    loadColorsLight();
    loadColorsDark();

    if (dark)
        colors = &colors_dark;
    else
        colors = &colors_light;

    setFocusPolicy(Qt::NoFocus);

    setWindowTitle(tr("Color scheme"));
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    QListWidget *list = new QListWidget(this);
    mainLayout->addWidget(list, 0, 0);
    QListWidgetItem *item;
    item = new QListWidgetItem(tr("Patch background"), list);
    item->setData(1, COLOR_PATCH_BACKGROUND);

    item = new QListWidgetItem(tr("Circuits: text"), list);
    item->setData(1, CIRV_COLOR_TEXT);

    item = new QListWidgetItem(tr("Circuits: text (disabled)"), list);
    item->setData(1, CIRV_COLOR_DISABLED_TEXT);

    item = new QListWidgetItem(tr("Circuits: comments"), list);
    item->setData(1, CIRV_COLOR_COMMENT);

    item = new QListWidgetItem(tr("Circuits: lines"), list);
    item->setData(1, CIRV_COLOR_LINE);

    item = new QListWidgetItem(tr("Circuits: circuit name"), list);
    item->setData(1, CIRV_COLOR_CIRCUIT_NAME);

    item = new QListWidgetItem(tr("Circuits: circuit name background"), list);
    item->setData(1, CIRV_COLOR_CIRCUIT_NAME_BG);

    item = new QListWidgetItem(tr("Circuits: comment background"), list);
    item->setData(1, CIRV_COLOR_COMMENT_BACKGROUND);

    item = new QListWidgetItem(tr("Circuits: odd row background"), list);
    item->setData(1, CIRV_COLOR_ODD_ROW);

    item = new QListWidgetItem(tr("Circuits: even row background"), list);
    item->setData(1, CIRV_COLOR_EVEN_ROW);

    item = new QListWidgetItem(tr("Circuits: input parameter"), list);
    item->setData(1, CIRV_COLOR_INPUT_JACK);

    item = new QListWidgetItem(tr("Circuits: output parameter"), list);
    item->setData(1, CIRV_COLOR_OUTPUT_JACK);

    item = new QListWidgetItem(tr("Circuits: unknown parameter"), list);
    item->setData(1, CIRV_COLOR_UNKNOWN_JACK);

    item = new QListWidgetItem(tr("Circuits: input parameter background"), list);
    item->setData(1, CIRV_COLOR_INPUT_JACK_BG);

    item = new QListWidgetItem(tr("Circuits: output parameter background"), list);
    item->setData(1, CIRV_COLOR_OUTPUT_JACK_BG);

    item = new QListWidgetItem(tr("Circuits: unknown parameter background"), list);
    item->setData(1, CIRV_COLOR_UNKNOWN_JACK_BG);

    item = new QListWidgetItem(tr("Circuits: disabled parameter background"), list);
    item->setData(1, CIRV_COLOR_DISABLED_JACK_BG);

    item = new QListWidgetItem(tr("Circuits: + and *"), list);
    item->setData(1, CIRV_COLOR_OPERATOR);

    item = new QListWidgetItem(tr("Circuits: + and * background"), list);
    item->setData(1, CIRV_COLOR_OPERATOR_BG);

    item = new QListWidgetItem(tr("Circuits: Selection"), list);
    item->setData(1, CIRV_COLOR_SELECTION);

    item = new QListWidgetItem(tr("Controller selector: background"), list);
    item->setData(1, CSEL_COLOR_BACKGROUND);

    item = new QListWidgetItem(tr("Controller selector: cursor"), list);
    item->setData(1, CSEL_COLOR_CURSOR);

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

    item = new QListWidgetItem(tr("Rack view: background"), list);
    item->setData(1, COLOR_RACK_BACKGROUND);

    item = new QListWidgetItem(tr("Rackview: register marker BG"), list);
    item->setData(1, RACV_REGMARKER_BACKGROUND);

    item = new QListWidgetItem(tr("Rackview: register marker"), list);
    item->setData(1, RACV_REGMARKER_PEN_COLOR);

    item = new QListWidgetItem(tr("Rackview: register hilites"), list);
    item->setData(1, RACV_REGHILITES_PEN_COLOR);

    item = new QListWidgetItem(tr("Rackview: register hilitesr lesser"), list);
    item->setData(1, RACV_REGHILITES_LESSER_PEN_COLOR);

    item = new QListWidgetItem(tr("Rackview: register label"), list);
    item->setData(1, RACV_COLOR_REGISTER_LABEL);

    item = new QListWidgetItem(tr("Rackview: register label BG"), list);
    item->setData(1, RACV_COLOR_LABEL_BG);

    item = new QListWidgetItem(tr("Rackview: drag arrow"), list);
    item->setData(1, RACV_COLOR_DRAGARROW);

    item = new QListWidgetItem(tr("Rackview: drag arrow, not locked at target"), list);
    item->setData(1, RACV_COLOR_DRAGARROW_UNLOCKED);

    item = new QListWidgetItem(tr("Rackview: drag arrow, not suitable"), list);
    item->setData(1, RACV_COLOR_DRAGARROW_UNSUITABLE);

    item = new QListWidgetItem(tr("Parameter selector: frame color"), list);
    item->setData(1, JSEL_COLOR_LINE);

    item = new QListWidgetItem(tr("Parameter selector: parameter BG"), list);
    item->setData(1, JSEL_COLOR_JACK_BACKGROUND);

    item = new QListWidgetItem(tr("Parameter selector: circuit name"), list);
    item->setData(1, JSEL_COLOR_CIRCUIT_NAME);

    item = new QListWidgetItem(tr("Parameter selector: circuit BG"), list);
    item->setData(1, JSEL_COLOR_CIRCUIT_BACKGROUND);

    item = new QListWidgetItem(tr("Parameter selector: inactive parameter"), list);
    item->setData(1, JSEL_COLOR_JACK_INACTIVE);

    item = new QListWidgetItem(tr("Parameter selector: inactive cursor"), list);
    item->setData(1, JSEL_COLOR_CURSOR_INACTIVE);

    item = new QListWidgetItem(tr("Parameter selector: description text"), list);
    item->setData(1, JSEL_COLOR_DESCRIPTION);

    item = new QListWidgetItem(tr("Parameter selector: description BG"), list);
    item->setData(1, JSEL_COLOR_DESCRIPTION_BACKGROUND);

    item = new QListWidgetItem(tr("Circuit chooser: title"), list);
    item->setData(1, CICH_COLOR_TITLE);

    item = new QListWidgetItem(tr("Circuit chooser: description"), list);
    item->setData(1, CICH_COLOR_DESCRIPTION);

    item = new QListWidgetItem(tr("Circuit chooser: ram size"), list);
    item->setData(1, CICH_RAMSIZE_COLOR);

    item = new QListWidgetItem(tr("Circuit chooser: circuit BG"), list);
    item->setData(1, CICH_COLOR_CIRCUIT_BACKGROUND);

    item = new QListWidgetItem(tr("Circuit chooser: background"), list);
    item->setData(1, CICH_COLOR_BACKGROUND);

    item = new QListWidgetItem(tr("Section manager: title background"), list);
    item->setData(1, PSM_COLOR_SECTION_BACKGROUND);

    item = new QListWidgetItem(tr("Section manager: title"), list);
    item->setData(1, PSM_COLOR_TITLE);

    item = new QListWidgetItem(tr("Statusbar: text"), list);
    item->setData(1, COLOR_STATUSBAR_TEXT);

    item = new QListWidgetItem(tr("Statusbar: background"), list);
    item->setData(1, COLOR_STATUSBAR_BACKGROUND);

    item = new QListWidgetItem(tr("Cable status: cable color"), list);
    item->setData(1, CSI_CABLE_COLOR);

    item = new QListWidgetItem(tr("Cable status: cable hilite color"), list);
    item->setData(1, CSI_CABLE_HILITE_COLOR);

    item = new QListWidgetItem(tr("Cable status: label"), list);
    item->setData(1, CSI_LABEL_COLOR);

    item = new QListWidgetItem(tr("Cable status: label background"), list);
    item->setData(1, CSI_LABEL_BACKGROUND);

    item = new QListWidgetItem(tr("Cable status: bad marker border"), list);
    item->setData(1, CSI_BAD_MARKER_BORDER);

    item = new QListWidgetItem(tr("Cable status: bad marker bg"), list);
    item->setData(1, CSI_BAD_MARKER_BACKGROUND);

    item = new QListWidgetItem(tr("Cable status: good marker border"), list);
    item->setData(1, CSI_GOOD_MARKER_BORDER);

    item = new QListWidgetItem(tr("Cable status: good marker bg"), list);
    item->setData(1, CSI_GOOD_MARKER_BACKGROUND);

    item = new QListWidgetItem(tr("Cable status: marker text"), list);
    item->setData(1, CSI_MARKER_TEXT_COLOR);

    colorDialog = new QColorDialog(this);
    colorDialog->setOption(QColorDialog::ShowAlphaChannel);
    colorDialog->setOption(QColorDialog::NoButtons);
    colorDialog->resize(200, 600);

    connect(list, &QListWidget::itemClicked, this, &ColorScheme::itemSelected);
    connect(colorDialog, &QColorDialog::currentColorChanged, this, &ColorScheme::colorChanged);
}

QColor ColorScheme::color(int index)
{
#ifdef QT_DEBUG
    QString key = settingsKey(index);
    // This is during the development phase if a new
    // colors just as been added but the header file
    // colors.h has not been generated, again, yet.
    if (settings.value(key).isValid())
        return settings.value(key).value<QColor>();
#endif

    // This is the normal way
    if (colors->contains(index))
        return (*colors)[index];

    // This case is if a new color has never been
    // adjusted.
    return QColor(128, 128, 128);
}
void ColorScheme::setColor(int index, const QColor &color)
{
    QString key = settingsKey(index);
    settings.setValue(key, color);
    (*colors)[index] = color;
}
void ColorScheme::hideEvent(QHideEvent *)
{
    colorDialog->hide();
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
void ColorScheme::dumpHeaderFile()
{
    QString filename = QString(COLOR_DEFINITION_FILE).arg(dark ? "dark" : "light");
    shout << "Creating " << filename;

    // First put all colors into the settings that are not
    // contained there. This is neccessary if someone is developing
    // but does not yet have colors settings.
    for (auto it = colors->constKeyValueBegin();
         it != colors->constKeyValueEnd();
         ++it)
    {
        QString key = settingsKey(it->first);
        if (!settings.contains(key))
            settings.setValue(key, it->second);
    }

    // Create a CPP file in order to hard code all the
    // colors for a release.
    QFile file(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream stream(&file);
    stream << "#include <QColor>\n";
    stream << "#include <QMap>\n";
    stream << QString("QMap<int, QColor> colors_%1;\n").arg(dark ? "dark" : "light");
    stream << QString("void loadColors%1()\n").arg(dark ? "Dark" : "Light");
    stream << "{\n";

    QString prefix("color/");
    prefix += dark ? "dark/" : "light/";
    for (auto &key: settings.allKeys()) {
        if (key.startsWith(prefix)) {
            QColor color = settings.value(key).value<QColor>();
            QString s = key.mid(prefix.length());
            int keyInt = s.toInt();

            QString line =  QString("    colors_%1.insert(%2, QColor(%3, %4, %5, %6));\n")
                     .arg(dark ? "dark" : "light")
                     .arg(keyInt)
                     .arg(color.red())
                     .arg(color.green())
                     .arg(color.blue())
                     .arg(color.alpha());

            stream << line;
        }
    }
    stream << "}\n";
    file.close();
}
bool ColorScheme::isDevelopment() const
{
#ifdef QT_DEBUG
    QFile file(QString(COLOR_DEFINITION_FILE).arg("dark"));
    return file.exists();
#else
    return false;
#endif
}

QString ColorScheme::settingsKey(int index)
{
    return QString("color/") + (dark ? "dark/" : "light/") + QString::number(index);
}
