#include "scalingimageprovider.h"

#include <QtCore/QFile>
#include <QtGui/QImageReader>

ScalingImageProvider::ScalingImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ScalingImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);

    /* Accepts id of the form PATH SCALING_IMAGE_PROVIDER_SEPARATOR SCALE_FACTOR
     * Examples:
     * - arrow.png@scaledBy@1
     * - arrow@2x@scaledBy@0.5
     */
    int separatorPosition = id.lastIndexOf(SCALING_IMAGE_PROVIDER_SEPARATOR);
    float scaleFactor = id.mid(separatorPosition+SCALING_IMAGE_PROVIDER_SEPARATOR.length()).toFloat();
    QString path = id.left(separatorPosition);
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        QImage image;
        QImageReader imageReader(&file);
        if (!qFuzzyCompare(scaleFactor, (float)1.0)) {
            QSize realSize = imageReader.size();
            imageReader.setScaledSize(realSize * scaleFactor);
        }
        imageReader.read(&image);
        *size = image.size();
        return image;
    } else {
        return QImage();
    }
}
