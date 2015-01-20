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
PropertyChange::PropertyChange(QObject *item, const char *property)
    : m_backedUp(false)
    , qmlProperty(item, property, qmlContext(item))
{
}
PropertyChange::~PropertyChange()
{
    restore(this);
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
    if (!change->m_backedUp) {
        change->backup.first = QQmlPropertyPrivate::setBinding(change->qmlProperty, 0);
        change->backup.second = change->qmlProperty.read();
        change->m_backedUp = true;
    }
    change->qmlProperty.write(value);
}

/*
 * Restore backed up value or binding.
 */
void PropertyChange::restore(PropertyChange *change)
{
    if (!change) {
        return;
    }
    if (change->m_backedUp) {
        // if there was a binding, restore it
        if (change->backup.first) {
            QQmlAbstractBinding *prevBinding = QQmlPropertyPrivate::setBinding(change->qmlProperty, change->backup.first);
            if (prevBinding && prevBinding != change->backup.first) {
                prevBinding->destroy();
            }
        } else {
            // there was no binding, restore previous value
            change->qmlProperty.write(change->backup.second);
        }
        change->m_backedUp = false;
    }
}
