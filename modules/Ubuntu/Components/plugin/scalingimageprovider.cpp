#include "scalingimageprovider.h"

#include <QtCore/QFile>
#include <QtGui/QImageReader>

ScalingImageProvider::ScalingImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage ScalingImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);

    /* Accepts id of the form SCALE_FACTOR/PATH
     * Examples:
     * - 1/arrow.png
     * - 0.5/arrow.png
     */
    int separatorPosition = id.indexOf("/");
    float scaleFactor = id.left(separatorPosition).toFloat();
    QString path = id.mid(separatorPosition+1);
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
