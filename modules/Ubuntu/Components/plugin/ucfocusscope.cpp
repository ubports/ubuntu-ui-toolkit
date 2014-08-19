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

#include "ucfocusscope.h"
#include "ucfocusscope_p.h"

UCFocusScopePrivate::UCFocusScopePrivate()
    : activeFocusOnMousePress(false)
    , pressed(false)
{
}

UCFocusScopePrivate::~UCFocusScopePrivate()
{
}

void UCFocusScopePrivate::init()
{
    Q_Q(UCFocusScope);
    q->setFlag(QQuickItem::ItemIsFocusScope);
}


void UCFocusScopePrivate::setFocusable(bool focus)
{
    Q_Q(UCFocusScope);
    q->setAcceptedMouseButtons(focus ? Qt::LeftButton : Qt::NoButton);
    q->setFiltersChildMouseEvents(focus);
    pressed = false;
}

bool UCFocusScopePrivate::isParentFocusable()
{
    Q_Q(UCFocusScope);
    if (!activeFocusOnMousePress) {
        return false;
    }
    QQuickItem *pl = q->parentItem();
    while (pl) {
        UCFocusScope *scope = qobject_cast<UCFocusScope*>(pl);
        if (scope) {
            UCFocusScopePrivate *pscope = UCFocusScopePrivate::get(scope);
            return pscope->isParentFocusable();
        }
        pl = pl->parentItem();
    }

    return true;
}


/*!
 * \qmltype FocusScope
 * \instantiates UCFocusScope
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.1
 * The component overrides the QtQuick's FocusScope component with additional
 * functionality for focusing.
 *
 * FocusScope does the same as QtQuick's FocusScope with the addition that also
 * handles focusing on mouse events. The component will be used if Ubuntu.Components
 * 1.1 is imported.
 *
 * Mouse focusing can be achieved by setting \l activeFocusOnMousePress, in which
 * case the component will grab the focus when the mouse is clicked over the component.
 * However if one of the FocusScope's ancestor FocusScope is having the property
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
 *     FocusScope {
 *         objectName: "passiveScope"
 *         width: parent.width
 *         height: units.gu(30)
 *         Rectangle {
 *             anchors.fill: parent
 *             color: "red"
 *             FocusScope {
 *                 objectName: "activeScope"
 *                 activeFocusOnMousePress: true
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
 * FocusScope cannot gain focus automaically if there is a MouseArea or other
 * component grabbing mouse events placed over it. In that case the focus must
 * be grabbed by the overlapping area explicitly either by calling the \l gainFocus
 * function or by forwarding mouse events to the covered FocusScope area.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     FocusScope {
 *         id: scope1
 *         width: parent.width
 *         height: units.gu(30)
 *         activeFocusOnMousePress: false
 *         Rectangle {
 *             focus: true
 *             anchors {
 *                 fill: parent
 *                 margins: units.gu(1)
 *             }
 *             color: !activeFocus ? "red" : "green"
 *             MouseArea {
 *                 anchors.fill: parent
 *                 onClicked: scope1.gainFocus()
 *             }
 *         }
 *     }
 *     FocusScope {
 *         id: scope2
 *         width: parent.width
 *         height: units.gu(30)
 *         activeFocusOnMousePress: false
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
 * \note Please observe that the order of importing the modules matters! If QtQuick
 * is imported after the Ubuntu Components, the QtQuick FocusScope will be used.
 * \qml
 * import Qubuntu.Components 1.1
 * import QtQuick 2.2
 *
 * FocusScope {
 *     // this will give parsing error
 *     activeFocusOnMousePress: true
 * }
 * \endqml
 *
 * So either change the order of imports or do named importing.
 * \qml
 * import Qubuntu.Components 1.1 as Toolkit
 * import QtQuick 2.2
 *
 * Toolkit.FocusScope {
 *     // no more parsing error
 *     activeFocusOnMousePress: true
 * }
 * \endqml
 */
UCFocusScope::UCFocusScope(QQuickItem *parent)
    : QQuickItem(*(new UCFocusScopePrivate), parent)
{
    Q_D(UCFocusScope);
    d->init();
}

UCFocusScope::UCFocusScope(UCFocusScopePrivate &dd, QQuickItem *parent)
    : QQuickItem(dd, parent)
{
    Q_D(UCFocusScope);
    d->init();
}

/*!
 * \qmlmethod void FocusScope::gainFocus()
 * \since Ubuntu.Components 1.1
 * The function is similar to \c forceActiveFocus except that it sets focus only
 * if all the ancestors have activeFocusOnMousePressed set.
 */
void UCFocusScope::gainFocus(Qt::FocusReason reason)
{
    //call overloaded only if can activate!
    Q_D(UCFocusScope);
    if (d->isParentFocusable()) {
        QQuickItem::forceActiveFocus(reason);
    }
}

/*!
 * \qmlproperty bool FocusScope::activeFocusOnMousePress
 * \since Ubuntu.Components 1.1
 *
 * The property specifies whether the FocusScope can gain focus on a mouse click
 * or not. When the value is true, the focus will be set on the FocusScope when
 * the mouse is clicked over the component. Note that click means that the mouse
 * is presed and released over the component. If the press happens inside the
 * component but the release outside, the component will not be focused.
 *
 * The default value is \c false.
 */
bool UCFocusScope::activefocusOnMousePress() const
{
    Q_D(const UCFocusScope);
    return d->activeFocusOnMousePress;
}
void UCFocusScope::setActiveFocusOnMousePress(bool value)
{
    Q_D(UCFocusScope);
    if (d->activeFocusOnMousePress == value)
        return;
    d->activeFocusOnMousePress = value;
    d->setFocusable(d->activeFocusOnMousePress);
    Q_EMIT activeFocusOnMousePressChanged();
}

// grab pressed state
void UCFocusScope::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    Q_D(UCFocusScope);
    d->pressed = d->isParentFocusable();
    event->setAccepted(d->pressed);
}
// check parent items for focusable and whether the mouse had been released over the
// component.
void UCFocusScope::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    Q_D(UCFocusScope);
    if (d->pressed && !hasActiveFocus() && contains(event->localPos()) && d->isParentFocusable()) {
        forceActiveFocus(Qt::MouseFocusReason);
    }
    d->pressed = false;
}

#include "moc_ucfocusscope.cpp"
