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
#include <QtCore/QRegularExpression>
#include <QtCore/qmath.h>

#define ENV_GRID_UNIT_PX "GRID_UNIT_PX"
#define DEFAULT_GRID_UNIT_PX 8

static float getenvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}


/*!
    \qmltype Units
    \instantiates UCUnits
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup resolution-independence
    \brief Units of measurement for sizes, spacing, margin, etc.

    Units provides facilities for measuring UI elements in a variety
    of units other than just pixels.

    A global instance of Units is exposed as the \b{units} context property.
    Example usage:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.2

    Item {
        width: units.gu(2)
        height: units.gu(5)
    }
    \endqml

    \sa {Resolution Independence}
*/
UCUnits::UCUnits(QObject *parent) :
    QObject(parent)
{
    m_gridUnit = getenvFloat(ENV_GRID_UNIT_PX, DEFAULT_GRID_UNIT_PX);
}

/*!
    \qmlproperty real Units::gridUnit

    The number of pixels 1 grid unit corresponds to.
*/
float UCUnits::gridUnit()
{
    return m_gridUnit;
}

void UCUnits::setGridUnit(float gridUnit)
{
    m_gridUnit = gridUnit;
    Q_EMIT gridUnitChanged();
}

/*!
    \qmlmethod real Units::dp(real value)

    Returns the number of pixels \a value density independent pixels correspond to.
*/
float UCUnits::dp(float value)
{
    const float ratio = m_gridUnit / DEFAULT_GRID_UNIT_PX;
    if (value <= 2.0) {
        // for values under 2dp, return only multiples of the value
        return qRound(value * qFloor(ratio));
    } else {
        return qRound(value * ratio);
    }
}

/*!
    \qmlmethod real Units::gu(real value)

    Returns the number of pixels \a value grid units correspond to.
*/
float UCUnits::gu(float value)
{
    return qRound(value * m_gridUnit);
}

QString UCUnits::resolveResource(const QUrl& url)
{
    if (url.isEmpty()) {
        return QString();
    }

    QString path = QQmlFile::urlToLocalFileOrQrc(url);

    if (path.isEmpty()) {
        return QString();
    }

    QFileInfo fileInfo(path);
    if (fileInfo.exists() && !fileInfo.isFile()) {
        return QString();
    }

    QString prefix = fileInfo.dir().absolutePath() + QDir::separator() + fileInfo.baseName();
    QString suffix = "." + fileInfo.completeSuffix();

    /* Use file with expected grid unit suffix if it exists.
       For example, if m_gridUnit = 10, look for resource@10.png.
    */

    path = prefix + suffixForGridUnit(m_gridUnit) + suffix;
    if (QFile::exists(path)) {
        return QString("1") + "/" + path;
    }

    /* No file with expected grid unit suffix exists.
       List all the files of the form fileBaseName@[0-9]*.fileSuffix and select
       the most appropriate one privileging downscaling high resolution assets
       over upscaling low resolution assets.

       The most appropriate file has a grid unit suffix greater than the target
       grid unit (m_gridUnit) yet as small as possible.
       If no file with a grid unit suffix greater than the target grid unit
       exists, then select one with a grid unit suffix as close as possible to
       the target grid unit.

       For example, if m_gridUnit = 10 and the available files are
       resource@9.png, resource@14.png and resource@18.png, the most appropriate
       file would be resource@14.png since it is above 10 and smaller
       than resource@18.png.
    */
    QStringList nameFilters;
    nameFilters << fileInfo.baseName() + "@[0-9]*" + suffix;
    QStringList files = fileInfo.dir().entryList(nameFilters, QDir::Files);

    if (!files.empty()) {
        float selectedGridUnitSuffix = gridUnitSuffixFromFileName(files.first());

        Q_FOREACH (const QString& fileName, files) {
            float gridUnitSuffix = gridUnitSuffixFromFileName(fileName);
            if ((selectedGridUnitSuffix >= m_gridUnit && gridUnitSuffix >= m_gridUnit && gridUnitSuffix < selectedGridUnitSuffix)
                || (selectedGridUnitSuffix < m_gridUnit && gridUnitSuffix > selectedGridUnitSuffix)) {
                selectedGridUnitSuffix = gridUnitSuffix;
            }
        }

        path = prefix + suffixForGridUnit(selectedGridUnitSuffix) + suffix;
        float scaleFactor = m_gridUnit / selectedGridUnitSuffix;
        return QString::number(scaleFactor) + "/" + path;
    }

    path = prefix + suffix;
    if (QFile::exists(path)) {
        return QString("1") + "/" + path;
    }

    return QString();
}

QString UCUnits::suffixForGridUnit(float gridUnit)
{
    return "@" + QString::number(gridUnit);
}

float UCUnits::gridUnitSuffixFromFileName(const QString& fileName)
{
    QRegularExpression re("^.*@([0-9]*).*$");
    QRegularExpressionMatch match = re.match(fileName);
    if (match.hasMatch()) {
        return match.captured(1).toFloat();
    } else {
        return 0;
    }
}
