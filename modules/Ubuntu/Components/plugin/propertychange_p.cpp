/*
 * Copyright 2014 Canonical Ltd.
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
 */

#include "propertychange_p.h"

#include <QtQml/private/qqmlabstractbinding_p.h>
#define foreach Q_FOREACH //workaround to fix private includes
#include <QtQml/private/qqmlbinding_p.h>     // for QmlBinding
#undef foreach

/*
 * The class is used to save properties and their bindings while the property is
 * altered temporarily.
 */
PropertyChange::PropertyChange(QObject *item, const char *property, bool autoBackup)
    : backedUp(false)
    , qmlProperty(item, property, qmlContext(item))
{
    if (autoBackup) {
        backup();
    }
}
PropertyChange::~PropertyChange()
{
    restore(this);
}

void PropertyChange::backup()
{
    if (!backedUp) {
        backupValue = qmlProperty.read();
        backedUp = true;
    }
}

/*
 * Sets a value to the property. Will back up the original values if it wasn't yet.
 * This function can be called many times, it will not destroy the backed up value/binding.
 */
void PropertyChange::setValue(PropertyChange *change, const QVariant &value)
{
    if (!change) {
        return;
    }
    change->backup();
    // write using QQmlPropertyPrivate so we can keep the bindings
    QQmlPropertyPrivate::write(change->qmlProperty,
                               value,
                               QQmlPropertyPrivate::BypassInterceptor | QQmlPropertyPrivate::DontRemoveBinding);
}

/*
 * Restore backed up value or binding.
 */
void PropertyChange::restore(PropertyChange *change)
{
    if (!change) {
        return;
    }
    if (change->backedUp) {
        // write using QQmlPropertyPrivate to keep the bindings
        QQmlPropertyPrivate::write(change->qmlProperty,
                                   change->backupValue,
                                   QQmlPropertyPrivate::BypassInterceptor | QQmlPropertyPrivate::DontRemoveBinding);
        change->backedUp = false;
    }
}
