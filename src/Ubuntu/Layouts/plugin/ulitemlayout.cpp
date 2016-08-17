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

#include "ulitemlayout.h"

/*!
 * \qmltype ItemLayout
 * \instantiates ULItemLayout
 * \inqmlmodule Ubuntu.Layouts 1.0
 * \ingroup ubuntu-layouts
 * \brief ItemLayout defines a new size & position of a single Item, for the purposes
 * of specifying layouts. This is to be used within a ConditionalLayout definition.
 *
 * Use ItemLayout to define the size and position of a single item within a layout.
 * The item name to be positioned should be specified in item property,
 * which should correspond to the string specified in \b Layouts.item attached to
 * an Item in the default layout. The Item with Layouts.item attached is reparented
 * into ItemLayout and it is anchor filled to it.
 *
 * Example:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Layouts 0.1
 *
 * Layouts {
 *     id: layouts
 *     width: units.gu(50)
 *     height: units.gu(50)
 *     layouts: [
 *         ConditionalLayout {
 *             when: layouts.width > units.gu(40)
 *             Row {
 *                 width: parent.width
 *                 ItemLayout {
 *                     item: "input"
 *                     width: units.gu(30)
 *                     height: units.gu(20)
 *                 }
 *                 TextArea {
 *                     text: "extra component"
 *                 }
 *             }
 *         }
 *     ]
 *     // default layout
 *     TextInput {
 *         anchors.fill: parent
 *         text: "input"
 *         Layouts.item: "input"
 *     }
 * }
 * \endqml
 */

ULItemLayout::ULItemLayout(QQuickItem *parent)
    : QQuickItem(parent)
{
}

ULItemLayout::~ULItemLayout()
{
}

/*!
 * \qmlproperty string ItemLayout::item
 * The property holds the item name to be positioned in the fragment.
 */
QString ULItemLayout::item() const
{
    return m_itemName;
}
void ULItemLayout::setItem(const QString &value)
{
    if (value == m_itemName) {
        return;
    }
    m_itemName = value;
}
