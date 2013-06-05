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

#include "ulconditionallayout.h"
#include "ulconditionallayout_p.h"
#include "ullayouts.h"
#include "ullayouts_p.h"
#include <QtQml/QQmlInfo>

ULConditionalLayoutAttached::ULConditionalLayoutAttached(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \qmlattachedproperty list<string> ConditionalLayout::items
 *
 * This attached property holds the list of conditional layout item identifiers
 * to be laid out by the container. Therefore it has meaning only if attached to
 * layout containers.
 *
 * The items are laid out in the order specified by the list.
 */
QStringList ULConditionalLayoutAttached::items() const
{
    return m_itemNames;
}
void ULConditionalLayoutAttached::setItems(const QStringList &names)
{
    if (names.isEmpty() || (names == m_itemNames)) {
        return;
    }
    m_itemNames = names;
    Q_EMIT itemsChanged();
}

/*!
 * \qmlattachedproperty string ConditionalLayout::item
 * This attached property identifies a component to be laid out. It is the unique
 * identifier of the component within a layout block. The property can be attached
 * to components to be laid out and has no effect when applied on layout containers.
 */
QString ULConditionalLayoutAttached::item() const
{
    return m_name;
}
void ULConditionalLayoutAttached::setItem(const QString &name)
{
    if (name.isEmpty() || name == m_name) {
        return;
    }
    m_name = name;
    Q_EMIT itemChanged();
}

/*!
 * \qmlattachedproperty real ConditionalLayout::width
 * The property holds the width to be applied for the items laid out within the
 * container.
 */
QQmlScriptString ULConditionalLayoutAttached::width() const
{
    return m_width;
}
void ULConditionalLayoutAttached::setWidth(const QQmlScriptString &width)
{
    m_width = width;
}

/*!
 * \qmlattachedproperty real ConditionalLayout::height
 * The property holds the height to be applied for the items laid out within the
 * container.
 */
QQmlScriptString ULConditionalLayoutAttached::height() const
{
    return m_height;
}
void ULConditionalLayoutAttached::setHeight(const QQmlScriptString &height)
{
    m_height = height;
}

/*!
 * \qmlattachedproperty real ConditionalLayout::scale
 * The property holds the scale to be applied for the items laid out within the
 * container.
 */
QQmlScriptString ULConditionalLayoutAttached::scale() const
{
    return m_scale;
}
void ULConditionalLayoutAttached::setScale(const QQmlScriptString &scale)
{
    m_scale = scale;
}

/*!
 * \qmlattachedproperty real ConditionalLayout::rotation
 * The property holds the rotation to be applied for the items laid out within the
 * container.
 */
QQmlScriptString ULConditionalLayoutAttached::rotation() const
{
    return m_rotation;
}
void ULConditionalLayoutAttached::setRotation(const QQmlScriptString &rotation)
{
    m_rotation = rotation;
}
