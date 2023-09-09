#ifndef CABLECOLORIZER_H
#define CABLECOLORIZER_H

#include <QImage>
#include <QIcon>
#include <QList>
#include <QMap>

class CableColorizer
{
    QList<QImage *>images;
    QList<QIcon *>icons;
    QImage *ghostImage;
    QMap<QString, int> cableColors;
    bool *used;

public:
    CableColorizer();
    ~CableColorizer();
    unsigned numColors() const { return images.count(); };
    float imageAspect() const;
    void colorizeAllCables(const QStringList &sl);
    const QImage *imageForCable(QString name);
    const QIcon *iconForCable(QString name);
    const QImage *ghostPlug() { return ghostImage; };

private:
    int chooseColorForCable(QString name);
    void loadImages();
    void clearAssignments();
    int imageIndex(QString name);
};

extern CableColorizer *the_cable_colorizer;

#endif // CABLECOLORIZER_H
