#include "units.h"

#include <QtCore>
#include <QtQml/QQmlContext>
#include <QtCore/QFileInfo>

#define ENV_SCALE_FACTOR "SCALE_FACTOR"

struct Category {
    QString suffix;
    float scaleFactor;
};

Category mdpi = { "", 1.0 };
Category hdpi = { "@1.5x", 1.5 };
Category xhdpi = { "@2.25x", 2.25 };
QList<Category> g_densityCategories;


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
    g_densityCategories.append(mdpi);
    g_densityCategories.append(hdpi);
    g_densityCategories.append(xhdpi);

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
    if (value.isEmpty()) {
        return "";
    }

    QString localPath = value.toLocalFile();
    if (localPath.isEmpty()) {
        return "";
    }

    QFileInfo fileInfo(localPath);
    if (fileInfo.exists() && !fileInfo.isFile()) {
        return "";
    }

    QString prefix = fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.baseName();
    QString suffix = "." + fileInfo.completeSuffix();
    QString path;

    path = prefix + suffixForScaleFactor(m_scaleFactor) + suffix;
    if (QFile::exists(path)) {
        return QString("1") + "/" + path;
    }

    path = prefix + xhdpi.suffix + suffix;
    if (QFile::exists(path)) {
        return QString::number(m_scaleFactor/xhdpi.scaleFactor) + "/" + path;
    }

    path = prefix + hdpi.suffix + suffix;
    if (QFile::exists(path)) {
        return QString::number(m_scaleFactor/hdpi.scaleFactor) + "/" + path;
    }

    path = prefix + mdpi.suffix + suffix;
    if (QFile::exists(path)) {
        return QString::number(m_scaleFactor/mdpi.scaleFactor) + "/" + path;
    }

    return "";
}

QString Units::suffixForScaleFactor(float scaleFactor)
{
    Q_FOREACH (Category category, g_densityCategories) {
        if (scaleFactor <= category.scaleFactor) {
            return category.suffix;
        }
    }

    return g_densityCategories.last().suffix;
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
