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
    , backupBinding(Q_NULLPTR)
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
        backupBinding = QQmlPropertyPrivate::setBinding(qmlProperty, Q_NULLPTR);
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
    change->qmlProperty.write(value);
}

/*
 * Same as setValue() but setting a binding.
 */
void PropertyChange::setBinding(PropertyChange *change, QQmlAbstractBinding *binding)
{
   if (!change) {
       return;
   }
   change->backup();
   QQmlAbstractBinding *prev = QQmlPropertyPrivate::setBinding(change->qmlProperty, binding);
   if (prev && prev != binding && prev != change->backupBinding) {
       prev->destroy();
   }
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
        if (change->qmlProperty.isValid()) {
            if (change->backupBinding) {
                QQmlAbstractBinding *prev = QQmlPropertyPrivate::setBinding(change->qmlProperty, change->backupBinding);
                if (prev != change->backupBinding && prev) {
                    prev->destroy();
                }
            } else {
                change->qmlProperty.write(change->backupValue);
            }
        }
        change->backedUp = false;
    }
}
