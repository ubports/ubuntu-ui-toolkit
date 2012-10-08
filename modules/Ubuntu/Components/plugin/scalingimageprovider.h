#ifndef SCALINGIMAGEPROVIDER_H
#define SCALINGIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>
#include <QtGui/QImage>

class ScalingImageProvider : public QQuickImageProvider
{
public:
    explicit ScalingImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // SCALINGIMAGEPROVIDER_H
