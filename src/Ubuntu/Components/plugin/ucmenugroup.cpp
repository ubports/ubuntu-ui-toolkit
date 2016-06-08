/*
 * Copyright 2016 Canonical Ltd.
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

#include "ucmenugroup.h"
#include "ucaction.h"
#include "ucactionlist.h"

/*!
 * \qmltype MenuGroup
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief Logical list of items for a menu.
 *
 * Example usage:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 * Menu {
 *     text: "Edit"
 *
 *     MenuGroup {
 *         Action { text: "Undo" }
 *         Action { text: "Redo" }
 *     }
 *
 *     MenuGroup {
 *         Action { text: "Cut" }
 *         ActionList {
 *             Action { text: "Copy" }
 *             Action { text: "Paste" }
 *         }
 *     }
 *
 *     MenuGroup {
 *         Action { text: "Select All" }
 *     }
 * }
 * \endqml
 */
/*!
 * \qmlsignal MenuGroup::added(Object action)
 * Signal called when a action is added to the list
 */

/*!
 * \qmlsignal MenuGroup::removed(Object action)
 * Signal called when a action is removed from the list
 */
/*!
 * \qmlsignal MenuGroup::changed()
 * Signal called when the contents of the group change,
 * including child content changes (eg. \l ActionList child add/remove)
 */
UCMenuGroup::UCMenuGroup(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlmethod MenuGroup::addObject(Object object)
 * Adds an Object to the list programatically.
 */
void UCMenuGroup::addObject(QObject *object)
{
    if (m_data.contains(object)) {
        return;
    }
    m_data.push_back(object);

    if (auto childGroup = qobject_cast<UCMenuGroup*>(object)) {
        connect(childGroup, &UCMenuGroup::changed, this, &UCMenuGroup::changed);
    } else if (auto actionList = qobject_cast<UCActionList*>(object)) {
        connect(actionList, &UCActionList::added, this, &UCMenuGroup::changed);
        connect(actionList, &UCActionList::removed, this, &UCMenuGroup::changed);
    }

    Q_EMIT added(object);
    Q_EMIT changed();
}

/*!
 * \qmlmethod MenuGroup::removeObject(Object object)
 * Removes an object from the list programatically.
 */
void UCMenuGroup::removeObject(QObject *object)
{
    if (!object) {
        return;
    }
    if (m_data.removeOne(object)) {
        disconnect(object, 0, this, 0);
        Q_EMIT removed(object);
        Q_EMIT changed();
    }
}

/*!

 * \qmlproperty list<Object> MenuGroup::data
 * \default
 * List of Objects in this MenuGroup
 * Note that when you set this property, the children of the MenuGroup will be ignored,
 * so do not set the list and define children.
 */
QQmlListProperty<QObject> UCMenuGroup::data()
{
    return QQmlListProperty<QObject>(this, 0, UCMenuGroup::append, UCMenuGroup::count, 0, UCMenuGroup::clear);
}

const QVector<QObject*> &UCMenuGroup::list() const
{
    return m_data;
}

void UCMenuGroup::append(QQmlListProperty<QObject> *list, QObject *object)
{
    UCMenuGroup *menuGroup = qobject_cast<UCMenuGroup*>(list->object);
    if (menuGroup) {
        menuGroup->addObject(object);
    }
}

void UCMenuGroup::clear(QQmlListProperty<QObject> *list)
{
    UCMenuGroup *menuGroup = qobject_cast<UCMenuGroup*>(list->object);
    if (menuGroup) {
        menuGroup->m_data.clear();
    }
}

int UCMenuGroup::count(QQmlListProperty<QObject> *list)
{
    UCMenuGroup *menuGroup = qobject_cast<UCMenuGroup*>(list->object);
    if (menuGroup) {
        return menuGroup->m_data.count();
    }
    return 0;
}
