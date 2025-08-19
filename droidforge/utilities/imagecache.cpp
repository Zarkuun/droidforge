#include "imagecache.h"

ImageCache *the_image_cache = 0;

ImageCache::ImageCache()
{
    the_image_cache = this;
}

const QImage &ImageCache::image(const QString &path)
{
    if (!images.contains(path))
        images[path] = QImage(path);

    return images[path];
}
