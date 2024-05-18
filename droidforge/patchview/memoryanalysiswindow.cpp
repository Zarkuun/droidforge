#include "memoryanalysiswindow.h"
#include "droidfirmware.h"

#include <QPushButton>

MemoryAnalysisWindow::MemoryAnalysisWindow(const Patch *patch, QWidget *parent)
    : SourceCodeEditor(tr("Memory Analysis"), "", parent, true /* read-only */)
    , patch(patch)
{
    QPushButton *bSortCount = new QPushButton(tr("Sort by count"));
    addButton(bSortCount);
    QPushButton *bSortAverage = new QPushButton(tr("Sort by average"));
    addButton(bSortAverage);
    QPushButton *bSortTotal = new QPushButton(tr("Sort by total"));
    addButton(bSortTotal);
    QPushButton *bSortName = new QPushButton(tr("Sort by name"));
    addButton(bSortName);

    connect(bSortCount, &QPushButton::pressed, this, [this]() { this->update(BY_COUNT); });
    connect(bSortAverage, &QPushButton::pressed, this, [this]() { this->update(BY_AVERAGE); });
    connect(bSortTotal, &QPushButton::pressed, this, [this]() { this->update(BY_TOTAL); });
    connect(bSortName, &QPushButton::pressed, this, [this]() { this->update(BY_NAME); });
}

void MemoryAnalysisWindow::showEvent(QShowEvent *)
{
    update(BY_TOTAL);
}

void MemoryAnalysisWindow::update(sortby_t sortby)
{
    typedef struct {
        QString name;
        unsigned base;
        unsigned count;
        unsigned total;
        double average;
    } circuitstats_t;
    QMap <QString,circuitstats_t> stats;

    unsigned total_count = 0;
    unsigned total_ram = 0;

    for (unsigned s=0; s<patch->numSections(); s++) {
        const PatchSection *section = patch->section(s);
        for (unsigned c=0; c<section->numCircuits(); c++) {
            const Circuit *circuit = section->circuit(c);
            QString name = circuit->getName();
            if (!stats.contains(name)) {
                stats[name] = {
                    name,
                    the_firmware->circuitMemoryFootprint(name),
                    0, // count
                    0, // total
                    0.0 // average
                };
            }
            unsigned memoryFootprint = circuit->memoryFootprint();
            stats[name].count += 1;
            stats[name].total += memoryFootprint;
            stats[name].average = (double)stats[name].total / stats[name].count;
            total_count ++;
            total_ram += memoryFootprint;
        }
    }
    QString text = tr("Memory analysis of patch \"%1\"\n\n").arg(patch->getTitle());
    text += tr("circuit                  count    base    avg    total     perc\n");

    QString separator = tr("───────────────────────────────────────────────────────────────\n");
    text += separator;

    QList<circuitstats_t> list;
    for (auto& e: stats)
        list.append(e);

    switch (sortby) {
    case BY_COUNT:
        std::sort(list.begin(), list.end(), [&](const circuitstats_t &a, const circuitstats_t &b) { return a.count > b.count;});
        break;

    case BY_AVERAGE:
        std::sort(list.begin(), list.end(), [&](const circuitstats_t &a, const circuitstats_t &b) { return a.average > b.average;});
        break;

    case BY_TOTAL:
        std::sort(list.begin(), list.end(), [&](const circuitstats_t &a, const circuitstats_t &b) { return a.total > b.total;});
        break;

    case BY_NAME:
        std::sort(list.begin(), list.end(), [&](const circuitstats_t &a, const circuitstats_t &b) { return a.name < b.name;});
        break;
    }

    for (auto& e: list) {
        text += QString("%1 %2 %3 %4 %5   %6%\n")
                    .arg(e.name, -22)
                    .arg(e.count, 7)
                    .arg(e.base, 7)
                    .arg(e.average, 6, 'f', 0)
                    .arg(e.total, 8)
                    .arg((double)e.total / total_ram * 100, 5, 'f', 1);
    }
    text += separator;
    text += QString("%1 %2 %3 %4 %5   100.0%\n")
                .arg(tr("total"), -22)
                .arg(total_count, 7)
                .arg(" ", 7)
                .arg((double)total_ram / total_count, 6, 'f', 0)
                .arg(total_ram, 8);

    text += "\n\n";
    text += tr("count: Number of circuits of this type\n");
    text += tr("base:  RAM usage of that type of circuit when no parameters are used\n");
    text += tr("avg:   average actual RAM usage per circuit\n");
    text += tr("total: total amount of RAM used by this circuit type\n");
    text += tr("perc:  percentage of used RAM contributed by this circuit type\n");

    updateContent(text);
}
