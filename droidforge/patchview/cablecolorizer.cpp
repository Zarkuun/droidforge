#include "cablecolorizer.h"

#include <QDir>

CableColorizer::CableColorizer()
    : ghostImage(0)
{
    loadImages();
    used = new bool[images.count()];
    clearAssignments();
}

CableColorizer::~CableColorizer()
{
    delete[] used;
    for (auto image: images)
        delete image;
    if (ghostImage)
        delete ghostImage;
}

float CableColorizer::imageAspect() const
{
    return float(images[0]->width()) / images[0]->height();
}

void CableColorizer::colorizeAllCables(const QStringList &sl)
{
    // We need to choose a color for each of the cables in
    // sl (which are all cables of a patch).
    clearAssignments();

    // Sort the list, so that we get a deterministic behaviour.
    QStringList sorted = sl;
    sorted.sort();

    for (auto &name: sorted)
        imageForCable(name); // ignore result, just assign
}
int CableColorizer::imageIndex(QString name)
{
    if (cableColors.contains(name))
        return cableColors[name];
    else
        return chooseColorForCable(name);
}
const QImage *CableColorizer::imageForCable(QString name)
{
    return images[imageIndex(name)];
}

const QIcon *CableColorizer::iconForCable(QString name)
{
    return icons[imageIndex(name)];
}

int CableColorizer::chooseColorForCable(QString name)
{
    uint hash = qHash(name);
    int index = hash % images.count();

    // Starting at our hash, find the next
    // empty cable image. If all are full, use
    // the hash anyway.
    for (int i=0; i<images.count(); i++) {
        if (used[index] == false)
            break;
        index = (index + 1) % images.count();
    }
    cableColors[name] = index;
    used[index] = true;
    return index;
}

void CableColorizer::loadImages()
{
    QDir imageDir(":images/cables");
    QDir iconDir(":images/cableicons");
    QStringList entries = imageDir.entryList();
    for (auto &entry: entries) {
        QImage *image = new QImage(imageDir.filePath(entry));
        if (entry == "ghost.png")
            ghostImage = image;
        else {
            images.append(image);
            QIcon *icon = new QIcon(iconDir.filePath(entry));
            icons.append(icon);
        }
    }
}

void CableColorizer::clearAssignments()
{
    cableColors.clear();
    for (unsigned i=0; i<images.count(); i++)
        used[i] = false;
}
