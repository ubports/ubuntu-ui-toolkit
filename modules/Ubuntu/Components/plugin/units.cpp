#include "units.h"

#include <QtCore>
#include <QtQml/QQmlContext>
#include <QtCore/QFileInfo>

#define ENV_SCALE_FACTOR "SCALE_FACTOR"

Bucket mdpi = { "mdpi", 160, 1.0 };
Bucket hdpi = { "hdpi", 240, 1.5 };
Bucket xhdpi = { "xhdpi", 340, 2.25 };
QList<Bucket> g_densityBuckets;

float selectBucket(float density, QString formFactor)
{
    if (formFactor == "desktop") {
        density = density * 1.45;
    } else {
        density = density;
    }

    int smallestDelta = 99999;
    float selectedScaleFactor;

    Q_FOREACH (Bucket bucket, g_densityBuckets) {
        int delta = qAbs(bucket.density - density);
        if (delta < smallestDelta) {
            smallestDelta = delta;
            selectedScaleFactor = bucket.scaleFactor;
        }
    }

    return selectedScaleFactor;
}

QString suffixForScaleFactor(float scaleFactor)
{
    Q_FOREACH (Bucket bucket, g_densityBuckets) {
        if (scaleFactor <= bucket.scaleFactor) {
            return bucket.suffix;
        }
    }

    return g_densityBuckets.last().suffix;
}

static float getenvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}


Units::Units(QObject *parent) :
    QObject(parent)
{
    g_densityBuckets.append(mdpi);
    g_densityBuckets.append(hdpi);
    g_densityBuckets.append(xhdpi);

    m_scaleFactor = getenvFloat(ENV_SCALE_FACTOR, 1.0);
}

float Units::scaleFactor()
{
    return m_scaleFactor;
}

void Units::setScaleFactor(float scaleFactor)
{
    m_scaleFactor = scaleFactor;
    Q_EMIT scaleFactorChanged();
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

    path = prefix + suffixForScaleFactor(m_scaleFactor) + suffix;
    if (QFile::exists(path)) {
        return path + "@x1";
    }

    path = prefix + xhdpi.suffix + suffix;
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor/xhdpi.scaleFactor);
    }

    path = prefix + hdpi.suffix + suffix;
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor/hdpi.scaleFactor);
    }

    path = prefix + mdpi.suffix + suffix;
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor/mdpi.scaleFactor);
    }

    path = fileInfo.filePath();
    if (QFile::exists(path)) {
        return path + "@x" + QString::number(m_scaleFactor);
    }

    return "";
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
