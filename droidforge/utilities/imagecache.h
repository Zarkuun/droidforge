#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QMap>
#include <QImage>


class ImageCache
{
    QMap <QString, QImage> images;

public:
    ImageCache();
    const QImage &image(const QString &path);
};

extern ImageCache *the_image_cache;

#endif // IMAGECACHE_H
