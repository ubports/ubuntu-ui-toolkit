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

#include "ucunits.h"

#include <QtQml/QQmlContext>
#include <QtQml/QQmlFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#define ENV_GRID_UNIT_PX "GRID_UNIT_PX"
#define DEFAULT_GRID_UNIT_PX 8
#define DEFAULT_RESOURCES_UNIT_PX 8
#define RESOURCES_UNIT_FILE "resources_unit"


static float getenvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}


UCUnits::UCUnits(QObject *parent) :
    QObject(parent)
{
    m_gridUnit = getenvFloat(ENV_GRID_UNIT_PX, DEFAULT_GRID_UNIT_PX);
    m_resourcesUnit = DEFAULT_RESOURCES_UNIT_PX;

    /* A file named RESOURCES_UNIT_FILE can be provided by the application
     * and contains the grid unit the resources have been designed to. */
    loadResourcesUnitFile(RESOURCES_UNIT_FILE);
}

bool UCUnits::loadResourcesUnitFile(QString fileName)
{
    QUrl unresolved = QUrl::fromLocalFile(fileName);
    fileName = m_baseUrl.resolved(unresolved).toLocalFile();

    QFile resourcesUnitFile(fileName);
    if (resourcesUnitFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        bool valid;
        int unit = resourcesUnitFile.readLine().trimmed().toInt(&valid);
        if (valid) {
            m_resourcesUnit = (float)unit;
        }
        return valid;
    }
    return false;
}

float UCUnits::gridUnit()
{
    return m_gridUnit;
}

void UCUnits::setGridUnit(float gridUnit)
{
    m_gridUnit = gridUnit;
    Q_EMIT gridUnitChanged();
}

void UCUnits::setBaseUrl(const QUrl& baseUrl)
{
    m_baseUrl = baseUrl;
}

float UCUnits::dp(float value)
{
    return qRound(value * m_gridUnit / DEFAULT_GRID_UNIT_PX);
}

float UCUnits::gu(float value)
{
    return qRound(value * m_gridUnit);
}

QString UCUnits::resolveResource(const QUrl& url)
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

    path = prefix + suffixForGridUnit(m_gridUnit) + suffix;
    float scaleFactor = m_gridUnit / m_resourcesUnit;

    if (QFile::exists(path)) {
        return QString("1") + "/" + path;
    }

    path = prefix + suffix;
    if (QFile::exists(path)) {
        return QString::number(scaleFactor) + "/" + path;
    }

    return "";
}

QString UCUnits::suffixForGridUnit(float gridUnit)
{
    return "@" + QString::number(gridUnit);
}
