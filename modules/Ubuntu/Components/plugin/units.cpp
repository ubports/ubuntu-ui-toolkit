#include "units.h"

#include <QtCore>
#include <QtQml/QQmlContext>
#include <QtCore/QFileInfo>

#define ENV_BUCKET "BUCKET"
#define ENV_SCALE_FACTOR "SCALE_FACTOR"

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

static float getenvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}

static QString getenvString(const char* name, const QString& defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    QString value = QString::fromLocal8Bit(stringValue);
    if (value.isEmpty()) {
        return defaultValue;
    } else {
        return value;
    }
}

Units::Units(QObject *parent) :
    QObject(parent)
{
    g_densityBuckets[mdpi.name] = mdpi;
    g_densityBuckets[hdpi.name] = hdpi;
    g_densityBuckets[xhdpi.name] = xhdpi;

    float scaleFactor = getenvFloat(ENV_SCALE_FACTOR, 1.0);
    QString bucket = getenvString(ENV_BUCKET, "mdpi");

    m_bucket = bucket;
    m_scaleFactor = scaleFactor;
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

QString Units::resolveResource(const QUrl& value)
{
    QFileInfo fileInfo(value.toLocalFile());
    QString prefix = fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.baseName() + "@";
    QString suffix = "." + fileInfo.completeSuffix();
    QString path;

    path = prefix + m_bucket + suffix;
    if (QFile::exists(path)) {
        return path + "@x1";
    }

    path = prefix + xhdpi.name + suffix;
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor/xhdpi.scaleFactor);
    }

    path = prefix + hdpi.name + suffix;
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor/hdpi.scaleFactor);
    }

    path = prefix + mdpi.name + suffix;
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor/mdpi.scaleFactor);
    }

    path = fileInfo.filePath();
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor);
    }

    return "";
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
