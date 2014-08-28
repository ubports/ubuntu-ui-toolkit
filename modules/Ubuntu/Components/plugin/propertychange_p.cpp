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
PropertyChange::PropertyChange()
    : m_backedUp(false)
{
}
PropertyChange::PropertyChange(QObject *item, const char *property, const QVariant &value)
    : m_backedUp(false)
{
    backupAndSet(item, property, value);
}
PropertyChange::~PropertyChange()
{
    restore();
}

/*
 * Backup the property value or binding and set a new value. Use restore to revert
 * to original value/binding.
 */
void PropertyChange::backupAndSet(QObject *item, const char *property, const QVariant &value)
{
    // if there was a backup previously, restore first
    restore();
    // set property
    qmlProperty = QQmlProperty(item, property, qmlContext(item));
    // backup property
    backup.first = QQmlPropertyPrivate::setBinding(qmlProperty, 0);
    backup.second = qmlProperty.read();
    // finally set the given value
    qmlProperty.write(value);
    m_backedUp = true;
}

/*
 * Restore backed up value or binding.
 */
void PropertyChange::restore()
{
    if (m_backedUp) {
        // if there was a binding, restore it
        if (backup.first) {
            QQmlAbstractBinding *prevBinding = QQmlPropertyPrivate::setBinding(qmlProperty, backup.first);
            if (prevBinding && prevBinding != backup.first) {
                prevBinding->destroy();
            }
        } else {
            // there was no binding, restore previous value
            qmlProperty.write(backup.second);
        }
        m_backedUp = false;
    }
}

