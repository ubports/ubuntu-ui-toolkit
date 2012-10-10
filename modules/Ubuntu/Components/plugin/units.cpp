/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "units.h"

#include <QtCore>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlFile>
#include <QtCore/QFileInfo>

#define ENV_SCALE_FACTOR "SCALE_FACTOR"

struct Category {
    QString suffix;
    float scaleFactor;
};

Category mdpi = { "", 1.0 };
Category hdpi = { "@1.5x", 1.5 };
Category xhdpi = { "@2x", 2.0 };
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

QString Units::resolveResource(const QUrl& url)
{
    if (url.isEmpty()) {
        return "";
    }

    QString path = QQmlFile::urlToLocalFileOrQrc(url);

    if (path.isEmpty()) {
        return "";
    }

    QFileInfo fileInfo(path);
    if (fileInfo.exists() && !fileInfo.isFile()) {
        return "";
    }

    QString prefix = fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.baseName();
    QString suffix = "." + fileInfo.completeSuffix();

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
