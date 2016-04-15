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

#ifndef PROPERTYCHANGE_P_H
#define PROPERTYCHANGE_P_H

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtQml/QQmlProperty>

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
#include <QtCore/QExplicitlySharedDataPointer>
#endif

class QQmlAbstractBinding;
class PropertyChange
{
public:
    PropertyChange(QObject *item, const char *property, bool autoBackup = false);
    ~PropertyChange();

    static void setValue(PropertyChange* change, const QVariant &value);
    static void setBinding(PropertyChange *change, QQmlAbstractBinding *binding);
    static void restore(PropertyChange* change);

    const QQmlProperty &property()
    {
        return qmlProperty;
    }

private:
    bool backedUp;
    QQmlProperty qmlProperty;
    QVariant backupValue;

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QExplicitlySharedDataPointer<QQmlAbstractBinding> backupBinding;
#else
    QQmlAbstractBinding *backupBinding;
#endif

    void backup();
};

#endif // PROPERTYCHANGE_P_H
