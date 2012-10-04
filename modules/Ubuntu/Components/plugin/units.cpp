#include "units.h"

#include <QtCore>
#include <QtQml/QQmlContext>

Bucket ldpi = { "ldpi", 120, 0.75 };
Bucket mdpi = { "mdpi", 160, 1.0 };
Bucket hdpi = { "hdpi", 240, 1.5 };
Bucket xhdpi = { "xhdpi", 320, 2.0 };
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
    g_densityBuckets[ldpi.name] = ldpi;
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


UnitsChangeListener::UnitsChangeListener(QQmlContext *context) : m_context(context)
{
    updateUnitsInstance();
}

void UnitsChangeListener::updateUnitsInstance()
{
    Units* units = new Units();
    Units* oldUnits = m_context->contextProperty("units").value<Units*>();
    // FIXME: do oldUnits require a delete?
    if (oldUnits) {
        units->setBucket(oldUnits->bucket());
    }
    m_context->setContextProperty("units", units);
    QObject::connect(units, SIGNAL(bucketChanged()),
                     this, SLOT(updateUnitsInstance()));
}
