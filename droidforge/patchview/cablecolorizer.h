#ifndef CABLECOLORIZER_H
#define CABLECOLORIZER_H

#include <QImage>
#include <QList>
#include <QMap>

class CableColorizer
{
    QList<QImage *>images;
    QMap<QString, int> cableColors;
    bool *used;

public:
    CableColorizer();
    ~CableColorizer();
    unsigned numColors() const { return images.count(); };
    float imageAspect() const;
    void colorizeAllCables(const QStringList &sl);
    const QImage *imageForCable(QString name);

private:
    int chooseColorForCable(QString name);
    void loadImages();
    void clearAssignments();
};

extern CableColorizer *the_cable_colorizer;

#endif // CABLECOLORIZER_H
