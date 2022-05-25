#include "cablecolorizer.h"

#include <QDir>

CableColorizer::CableColorizer()
    : nextFreeIndex(0)
{
    loadImages();
}

CableColorizer::~CableColorizer()
{
    for (auto image: images)
        delete image;
}

float CableColorizer::imageAspect() const
{
    return float(images[0]->width()) / images[0]->height();
}

void CableColorizer::colorizeAllCables(const QStringList &sl)
{
    cableColors.clear();
    for (auto &name: sl)
        imageForCable(name);

    // TODO: Now remove all entries that are not contained
    // in sl
}

const QImage *CableColorizer::imageForCable(QString name)
{
    int index;
    if (cableColors.contains(name))
        index = cableColors[name];
    else
        index = chooseColorForCable(name);
    return images[index];
}

int CableColorizer::chooseColorForCable(QString name)
{
    // TODO: Beim zweiten Durchlauf schauen, ob es belegte
    // oder Löcher gibt. Die Löcher auffüllen. Wenn alle
    // voll sind, dann erst Doppelbelegungen machen.
    int index = nextFreeIndex++;
    if (nextFreeIndex >= images.count())
        nextFreeIndex = 0;
    cableColors[name] = index;
    qDebug() << name << "->" << index;
    return index;
}

void CableColorizer::loadImages()
{
    QDir dir(":images/cables");
    QStringList entries = dir.entryList();
    for (auto &entry: entries) {
        QImage *image = new QImage(dir.filePath(entry));
        images.append(image);
        qDebug() << "LOAD" << entry << "=" << images.count() - 1;
    }
}
