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


void UCFocusScopePrivate::setFocusable()
{
    Q_Q(UCFocusScope);
    q->setAcceptedMouseButtons(activeFocusOnMousePress ? Qt::LeftButton : Qt::NoButton);
    q->setFiltersChildMouseEvents(activeFocusOnMousePress);
    pressed = false;
}

bool UCFocusScopePrivate::isParentNonFocusable()
{
    Q_Q(UCFocusScope);
    if (!activeFocusOnMousePress) {
        qDebug() << "BREAK" << q;
        return false;
    }
    QQuickItem *pl = q->parentItem();
    while (pl) {
        UCFocusScope *scope = qobject_cast<UCFocusScope*>(pl);
        if (scope) {
            UCFocusScopePrivate *pscope = UCFocusScopePrivate::get(scope);
            return pscope->isParentNonFocusable();
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
 * Button.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.1
 *
 * MainView {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     Page {
 *         title: "Non-focusable input"
 *         activeFocusOnMousePress: false
 *         TextField {
 *             text: "Focus won't be taken away."
 *         }
 *         Button {
 *             text: "Press me"
 *         }
 *     }
 * }
 * \endqml
 */
UCFocusScope::UCFocusScope(QQuickItem *parent)
    : QQuickItem(*(new UCFocusScopePrivate), parent)
{
    Q_D(UCFocusScope);
    d->init();
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
SIMPLE_PRIVATE_PROPERTY(UCFocusScope, bool, activeFocusOnMousePress, d->setFocusable())

void UCFocusScope::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    Q_D(UCFocusScope);
    d->pressed = contains(event->localPos()) && d->isParentNonFocusable();
}

void UCFocusScope::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    Q_D(UCFocusScope);
    if (d->pressed && contains(event->localPos())) {
        forceActiveFocus(Qt::MouseFocusReason);
    }
}


#include "moc_ucfocusscope.cpp"
