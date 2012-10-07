#include "units.h"

#include <QtCore>
#include <QtQml/QQmlContext>

Bucket mdpi = { "mdpi", 160, 1.0 };
Bucket hdpi = { "hdpi", 240, 1.5 };
Bucket xhdpi = { "xhdpi", 340, 2.25 };
QHash<QString, Bucket> g_densityBuckets;

QString selectBucket(float density, QString formFactor)
{
    if (formFactor == "desktop") {
        density = density * 1.45;
    } else {
        density = density;
    }

    int smallestDelta = 99999;
    QString selectedBucketName;

    Q_FOREACH (Bucket bucket, g_densityBuckets) {
        int delta = qAbs(bucket.density - density);
        if (delta < smallestDelta) {
            smallestDelta = delta;
            selectedBucketName = bucket.name;
        }
    }

    return selectedBucketName;
}


Units::Units(QObject *parent) :
    QObject(parent)
{
    g_densityBuckets[mdpi.name] = mdpi;
    g_densityBuckets[hdpi.name] = hdpi;
    g_densityBuckets[xhdpi.name] = xhdpi;

    setBucket(selectBucket(113, "desktop"));
}

float Units::scaleFactor()
{
    return m_scaleFactor;
}

QString Units::bucket()
{
    return m_bucket;
}

float Units::dp(float value)
{
    return qFloor(value * m_scaleFactor);
}

void Units::setBucket(QString bucketName)
{
    m_bucket = bucketName;
    m_scaleFactor = g_densityBuckets[bucketName].scaleFactor;
    Q_EMIT scaleFactorChanged();
    Q_EMIT bucketChanged();
}


ContextPropertyChangeListener::ContextPropertyChangeListener(QQmlContext *context,QString contextProperty) :
    m_context(context),
    m_contextProperty(contextProperty)
{
}

void ContextPropertyChangeListener::updateContextProperty()
{
    QVariant value = m_context->contextProperty(m_contextProperty);
    m_context->setContextProperty(m_contextProperty, value);
}
