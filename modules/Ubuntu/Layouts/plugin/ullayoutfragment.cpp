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
#include <QDebug>

/*!
 * \qmltype LayoutFragment
 * \instantiates ULLayoutFragment
 * \inqmlmodule Ubuntu.Layouts 0.1
 * \ingroup ubuntu-layouts
 * \brief LayoutFragment is a pseudo-Item providing re-positioning of a single item within a layout.
 *
 * The item name to be positioned should be specified in itemName property, which
 * should correspond to the “ConditionalLayout.name” of an Item in the document.
 * The LayoutFragment itself isn’t visible, but the properties defined on it (anchors,
 * positioning, etc.) are applied to the Item with ConditionalLayout.name - so
 * effectively is a placeholder for the actual Item.
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
 *         ConditionalLayout.name: "input"
 *     }
 * }
 * \endqml
 */

ULLayoutFragment::ULLayoutFragment(QQuickItem *parent)
    : QQuickItem(parent)
{
    connect(this, SIGNAL(xChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(yChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(zChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(widthChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(heightChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(opacityChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(clipChanged(bool)), SLOT(markPropertyChange()));
    connect(this, SIGNAL(rotationChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(scaleChanged()), SLOT(markPropertyChange()));
    connect(this, SIGNAL(transformOriginChanged(TransformOrigin)), SLOT(markPropertyChange()));
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

/*!
 * \internal
 * \brief ULLayoutFragment::changedProperties
 */
QStringList ULLayoutFragment::changedProperties() const
{
    return m_changedProperties.toList();
}
/*!
 * \internal
 */
void ULLayoutFragment::markPropertyChange()
{
    int signalIndex = senderSignalIndex();
    const QMetaMethod signalMoc = sender()->metaObject()->method(signalIndex);
    QString property = QString(signalMoc.name()).remove("Changed");
    if (!m_changedProperties.contains(property)) {
        qDebug() << "fragment sets " << property;
        m_changedProperties << property;
    }
}
