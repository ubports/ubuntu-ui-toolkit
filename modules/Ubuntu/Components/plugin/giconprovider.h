#ifndef GICONPROVIDER_H
#define GICONPROVIDER_H

#include <QQuickImageProvider>

class GIconProvider : public QQuickImageProvider
{
public:
    GIconProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif
