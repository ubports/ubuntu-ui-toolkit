/*
 * Copyright 2013 Canonical Ltd.
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

#include "ullayouts.h"
#include "ullayouts_p.h"
#include <QtQml/QQmlInfo>

ULLayoutsAttached::ULLayoutsAttached(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlattachedproperty string Layouts::item
 * This attached property identifies a component to be laid out. It is the unique
 * identifier of the component within a layout block. The property can be attached
 * to components to be laid out and has no effect when applied on layout containers.
 */
QString ULLayoutsAttached::item() const
{
    return m_name;
}
void ULLayoutsAttached::setItem(const QString &name)
{
    if (name.isEmpty() || name == m_name) {
        return;
    }
    m_name = name;
    Q_EMIT itemChanged();
}

