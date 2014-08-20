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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucstyleditembase.h"
#include "ucstyleditembase_p.h"

UCStyledItemBasePrivate::UCStyledItemBasePrivate()
    : activeFocusOnPress(false)
    , pressed(false)
{
}

UCStyledItemBasePrivate::~UCStyledItemBasePrivate()
{
}

void UCStyledItemBasePrivate::init()
{
    Q_Q(UCStyledItemBase);
    q->setFlag(QQuickItem::ItemIsStyledItemBase);
}


void UCStyledItemBasePrivate::setFocusable(bool focus)
{
    Q_Q(UCStyledItemBase);
    q->setAcceptedMouseButtons(focus ? (Qt::LeftButton | Qt::RightButton | Qt::MiddleButton) : Qt::NoButton);
    q->setFiltersChildMouseEvents(focus);
    pressed = false;
}

bool UCStyledItemBasePrivate::isParentFocusable()
{
    Q_Q(UCStyledItemBase);
    if (!activeFocusOnPress) {
        return false;
    }
    QQuickItem *pl = q->parentItem();
    while (pl) {
        UCStyledItemBase *scope = qobject_cast<UCStyledItemBase*>(pl);
        if (scope) {
            UCStyledItemBasePrivate *pscope = UCStyledItemBasePrivate::get(scope);
            return pscope->isParentFocusable();
        }
        pl = pl->parentItem();
    }

    return true;
}


/*!
 * \qmltype StyledItemBase
 * \instantiates UCStyledItemBase
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.1
 * \internal
 * \qmlabstract
 * The component overrides the QtQuick's StyledItemBase component with additional
 * functionality for focusing.
 *
 * StyledItemBase does the same as QtQuick's FocusScope with the addition that also
 * handles focusing on mouse events. The component will be used if Ubuntu.Components
 * 1.1 is imported.
 *
 * Mouse focusing can be achieved by setting \l activeFocusOnPress, in which
 * case the component will grab the focus when the mouse is clicked over the component.
 * However if one of the StyledItemBase's ancestor StyledItemBase is having the property
 * value false, the focus will not be gained automatically.
 *
 * In the following example the TextField will stay focused when clicked on the
 * red rectangle.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     StyledItemBase {
 *         objectName: "passiveScope"
 *         width: parent.width
 *         height: units.gu(30)
 *         Rectangle {
 *             anchors.fill: parent
 *             color: "red"
 *             StyledItemBase {
 *                 objectName: "activeScope"
 *                 activeFocusOnPress: true
 *                 anchors.fill: parent
 *             }
 *         }
 *     }
 *
 *     TextField {
 *         id: input
 *         text: "The input stays focus even if red box is clicked"
 *     }
 *
 *     Component.onCompleted: input.forceActiveFocus()
 *
 *     Connections {
 *         target: window
 *         onActiveFocusItemChanged: console.debug("focus on", window.activeFocusItem)
 *     }
 * }
 * \endqml
 *
 * StyledItemBase cannot gain focus automaically if there is a MouseArea or other
 * component grabbing mouse events placed over it. In that case the focus must
 * be grabbed by the overlapping area explicitly either by calling the \l requestFocus
 * function or by forwarding mouse events to the covered StyledItemBase area.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     StyledItemBase {
 *         id: scope1
 *         width: parent.width
 *         height: units.gu(30)
 *         activeFocusOnPress: false
 *         Rectangle {
 *             focus: true
 *             anchors {
 *                 fill: parent
 *                 margins: units.gu(1)
 *             }
 *             color: !activeFocus ? "red" : "green"
 *             MouseArea {
 *                 anchors.fill: parent
 *                 onClicked: scope1.requestFocus()
 *             }
 *         }
 *     }
 *     StyledItemBase {
 *         id: scope2
 *         width: parent.width
 *         height: units.gu(30)
 *         activeFocusOnPress: false
 *         Rectangle {
 *             focus: true
 *             anchors {
 *                 fill: parent
 *                 margins: units.gu(1)
 *             }
 *             color: !activeFocus ? "red" : "green"
 *             MouseArea {
 *                 anchors.fill: parent
 *                 Mouse.forwardTo: [scope2]
 *             }
 *         }
 *     }
 * }
 * \endqml
 */
UCStyledItemBase::UCStyledItemBase(QQuickItem *parent)
    : QQuickItem(*(new UCStyledItemBasePrivate), parent)
{
    Q_D(UCStyledItemBase);
    d->init();
}

UCStyledItemBase::UCStyledItemBase(UCStyledItemBasePrivate &dd, QQuickItem *parent)
    : QQuickItem(dd, parent)
{
    Q_D(UCStyledItemBase);
    d->init();
}

/*!
 * \qmlmethod void StyledItemBase::requestFocus()
 * \since Ubuntu.Components 1.1
 * The function is similar to \c forceActiveFocus except that it sets focus only
 * if all the ancestors have activeFocusOnPressed set.
 */
void UCStyledItemBase::requestFocus(Qt::FocusReason reason)
{
    //call overloaded only if can activate!
    Q_D(UCStyledItemBase);
    if (d->isParentFocusable()) {
        QQuickItem::forceActiveFocus(reason);
    }
}

/*!
 * \qmlproperty bool StyledItemBase::activeFocusOnPress
 * \since Ubuntu.Components 1.1
 *
 * The property specifies whether the StyledItem can gain focus on a mouse
 * press/touch or not. When the value is true, the focus will be set on the component
 * when the mouse is pressed over it or touched. However if one of the component's
 * ancestor StyledItem or derived is having the property value false, the focus
 * will not be gained automatically.
 *
 * In the following example the TextField will stay focused when clicked on the
 * red rectangle.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     StyledItem {
 *         objectName: "passiveScope"
 *         width: parent.width
 *         height: units.gu(30)
 *         Rectangle {
 *             anchors.fill: parent
 *             color: "red"
 *             StyledItem {
 *                 objectName: "activeScope"
 *                 activeFocusOnPress: true
 *                 anchors.fill: parent
 *             }
 *         }
 *     }
 *
 *     TextField {
 *         id: input
 *         text: "The input stays focus even if red box is clicked"
 *     }
 *
 *     Component.onCompleted: input.forceActiveFocus()
 *
 *     Connections {
 *         target: window
 *         onActiveFocusItemChanged: console.debug("focus on", window.activeFocusItem)
 *     }
 * }
 * \endqml
 *
 * StyledItem cannot gain focus automaically if there is a MouseArea or other
 * component grabbing mouse events placed over it. In that case the focus must
 * be grabbed by the overlapping area explicitly either by calling the \l requestFocus
 * function or by forwarding mouse events to the covered StyledItem area.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     StyledItem {
 *         id: scope1
 *         width: parent.width
 *         height: units.gu(30)
 *         activeFocusOnPress: false
 *         Rectangle {
 *             focus: true
 *             anchors {
 *                 fill: parent
 *                 margins: units.gu(1)
 *             }
 *             color: !activeFocus ? "red" : "green"
 *             MouseArea {
 *                 anchors.fill: parent
 *                 onClicked: scope1.requestFocus()
 *             }
 *         }
 *     }
 *     StyledItem {
 *         id: scope2
 *         width: parent.width
 *         height: units.gu(30)
 *         activeFocusOnPress: false
 *         Rectangle {
 *             focus: true
 *             anchors {
 *                 fill: parent
 *                 margins: units.gu(1)
 *             }
 *             color: !activeFocus ? "red" : "green"
 *             MouseArea {
 *                 anchors.fill: parent
 *                 Mouse.forwardTo: [scope2]
 *             }
 *         }
 *     }
 * }
 * \endqml
 *
 * The default value is \c false.
 */
bool UCStyledItemBase::activefocusOnPress() const
{
    Q_D(const UCStyledItemBase);
    return d->activeFocusOnPress;
}
void UCStyledItemBase::setActiveFocusOnPress(bool value)
{
    Q_D(UCStyledItemBase);
    if (d->activeFocusOnPress == value)
        return;
    d->activeFocusOnPress = value;
    d->setFocusable(d->activeFocusOnPress);
    Q_EMIT activeFocusOnPressChanged();
}

// grab pressed state
void UCStyledItemBase::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    Q_D(UCStyledItemBase);
    d->pressed = d->isParentFocusable();
    event->setAccepted(d->pressed);
}
// check parent items for focusable and whether the mouse had been released over the
// component.
void UCStyledItemBase::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    Q_D(UCStyledItemBase);
    if (d->pressed && !hasActiveFocus() && contains(event->localPos()) && d->isParentFocusable()) {
        forceActiveFocus(Qt::MouseFocusReason);
    }
    d->pressed = false;
}

#include "moc_ucstyleditembase.cpp"
