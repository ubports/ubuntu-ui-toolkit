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

#include <QDebug>

/*!
 * \qmltype ConditionalLayout
 * \instantiates ULConditionalLayoutAttached
 * \inqmlmodule Ubuntu.Layouts 0.1
 * \ingroup ubuntu-layouts
 * \brief ConditionalLayout attached properties are attached either to layout
 * containers (fragments) or components to be laid out.
 */
ULConditionalLayoutAttached::ULConditionalLayoutAttached(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \qmlattachedproperty list<string> ULConditionalLayoutAttached::items
 * List of item names to be included in the layout section. This property can be
 * attached only to layout section declarations.
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
 * \qmlattachedproperty string ULConditionalLayoutAttached::itemName
 * The property can be attached to components to be laid out and represents the
 * layout section to be attached to.
 */
QString ULConditionalLayoutAttached::name() const
{
    return m_name;
}
void ULConditionalLayoutAttached::setName(const QString &name)
{
    if (name.isEmpty() || name == m_name) {
        return;
    }
    m_name = name;
    Q_EMIT nameChanged();
}

/*!
 * \qmlattachedproperty real ConditionalLayout::width
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
 */
QQmlScriptString ULConditionalLayoutAttached::rotation() const
{
    return m_rotation;
}
void ULConditionalLayoutAttached::setRotation(const QQmlScriptString &rotation)
{
    m_rotation = rotation;
}
