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

#include "ullayoutfragment.h"

/*!
 * \qmltype LayoutFragment
 * \instantiates ULLayoutFragment
 * \inqmlmodule Ubuntu.Layouts 0.1
 * \ingroup ubuntu-layouts
 * \brief LayoutFragment is a pseudo-Item providing re-positioning of a single
 * item within a layout.
 *
 * The item name to be positioned should be specified in item property, which
 * should correspond to the “ConditionalLayout.item” of an Item in the document.
 * The Item with ConditionalLayout.item is reparented into LayoutFragment and it
 * is anchor filled to it.
 *
 * Example:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Layouts 0.1
 *
 * Layouts {
 *     id: layouts
 *     width: units.gu(50)
 *     height: units.gu(50)
 *     layouts: [
 *         ConditionalLayout {
 *             when: layouts.width > units.gu(40)
 *             Item {
 *                 Row {
 *                     width: parent.width
 *                     LayoutFragment {
 *                         item: "input"
 *                         width: units.gu(30)
 *                         height: units.gu(20)
 *                     }
 *                     TextArea {
 *                         text: "extra component"
 *                     }
 *                 }
 *             }
 *         }
 *     ]
 *     // default layout
 *     TextInput {
 *         anchors.fill: parent
 *         text: "input"
 *         ConditionalLayout.item: "input"
 *     }
 * }
 * \endqml
 */

ULLayoutFragment::ULLayoutFragment(QQuickItem *parent)
    : QQuickItem(parent)
{
}

ULLayoutFragment::~ULLayoutFragment()
{}

/*!
 * \qmlproperty string Fragment::item
 * The property holds the item name to be positioned in the fragment.
 */
QString ULLayoutFragment::item() const
{
    return m_itemName;
}
void ULLayoutFragment::setItem(const QString &value)
{
    if (value.isEmpty() || (value == m_itemName)) {
        return;
    }
    m_itemName = value;
}
