/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucimportversionchecker_p.h"
#include "ucnamespace.h"

#include <QtQuick>
#include <QtQml/private/qqmldata_p.h>
#include <QtQml/private/qqmlcontext_p.h>
#include <QtQml/private/qqmlpropertycache_p.h>
#include <QtQml/private/qqmlmetatype_p.h>

/*!
 * \internal
 *
 * The function returns the version the module is imported with based on a QML
 * component instance. It only checks till reaches 1.2 version. Implementations
 * must implement the propertyForVersion method and must probvide a property for
 * each version requested.
 */
quint16 UCImportVersionChecker::importVersion(QObject *object)
{
    QQmlData *data = QQmlData::get(object);
    Q_ASSERT(data);
    QQmlContextData *cdata = QQmlContextData::get(qmlContext(object));
    Q_ASSERT(cdata);
    QQmlEngine *engine = qmlEngine(object);
    Q_ASSERT(engine);

    // start from the highest available version till we reach 1.2
    for (quint16 minor = MINOR_VERSION(LATEST_UITK_VERSION); minor > 2; minor--) {
        quint16 version = BUILD_VERSION(1, minor);
        const QString property(propertyForVersion(version));
        Q_ASSERT(!property.isEmpty());
        QQmlPropertyData l;
        QQmlPropertyData *p = QQmlPropertyCache::property(engine, object, property, cdata, l);
        if (data->propertyCache->isAllowedInRevision(p)) {
            return version;
        }
    }
    // if none found, simply fall back to 1.2
    return BUILD_VERSION(1, 2);
}
