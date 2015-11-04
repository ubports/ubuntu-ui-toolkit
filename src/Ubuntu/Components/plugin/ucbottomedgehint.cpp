/*
 * Copyright 2015 Canonical Ltd.
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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucbottomedgehint.h"
#include "ucstyleditembase_p.h"
#include <QtQml/private/qqmlproperty_p.h>

/*!
    \qmltype BottomEdgeHint
    \inqmlmodule Ubuntu.Components 1.3
    \ingroup ubuntu
    \inherits StyledItem
    \brief The BottomEdgeHint shows the availability of extra features
    available from the bottom edge of the application.

    It displays either a label or an icon at the bottom of the application.

    It has 4 states: Hidden, Idle, Active and Locked. When Idle, part of it is
    still visible hinting the existence of the bottom edge.

    When used with a mouse it acts like a button. The typical action associated
    with clicking on it should be revealing the extra features provided by the
    bottom edge. However, the click can only happen if the hint is in \e Locked
    state.

    Example:
    \qml
    BottomEdgeHint {
        id: bottomEdgeHint
        text: i18n.tr("Favorites")
        onClicked: revealBottomEdge()
    }
    \endqml

    The component is styled through \b BottomEdgeHintStyle.
*/
UCBottomEdgeHint::UCBottomEdgeHint(QQuickItem *parent)
    : UCStyledItemBase(parent)
    , m_flickable(Q_NULLPTR)
    , m_state(Idle)
{
    /*
     * we cannot use setStyleName as that will trigger style loading
     * and the qmlEngine is not known at this phase of the of the initialization
     * Therefore we simply set the style name default. Style loading will
     * happen during component completion.
     */
    UCStyledItemBasePrivate::get(this)->styleDocument = "BottomEdgeHintStyle";
}

void UCBottomEdgeHint::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCStyledItemBase::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        QQmlProperty bottomAnchors(this, "anchors.bottom", qmlContext(this));
        if (data.item && !QQmlPropertyPrivate::binding(bottomAnchors)) {
            QQuickAnchors *anchors = QQuickItemPrivate::get(this)->anchors();
            anchors->setBottom(QQuickItemPrivate::get(data.item)->bottom());
        }
    }
}

/*!
  \qmlsignal void BottomEdgeHint::clicked()
   This handler is called when there is a mouse click on the BottomEdgeHint
   and the BottomEdgeHint is not disabled.
*/

/*!
  \qmlproperty string BottomEdgeHint::text
  The label displayed by the BottomEdgeHint.
 */

/*!
  \qmlproperty url BottomEdgeHint::iconSource
  The icon displayed by the BottomEdgeHint.

  This is the URL of any image file.
  If both iconSource and \l iconName are defined, \l iconName will be ignored.
 */

/*!
  \qmlproperty string BottomEdgeHint::iconName
  The icon associated with the BottomEdgeHint in the icon theme.

  If both \l iconSource and iconName are defined, iconName will be ignored.
 */

/*!
  \qmlproperty Flickable BottomEdgeHint::flickable
  The property holds the flickable, which when flicked hides the hint.
  \e Hidden state is reached when this property is set to a Flickable
  which is flicking or moving. It is recommended to set the property
  when the hint is placed above a flickable content. Defaults to null.
  */

/*!
  \qmlproperty enum BottomEdgeHint::state
  BottomEdgeHint can take 4 states of visibility: \e Hidden, \e Idle, \e Active
  and \e Locked.
  \table
  \header
    \li State
    \li Description
  \row
    \li Hidden
    \li The hint is not shown at all and cannot be activated.
  \row
    \li Idle
    \li The hint is in a state where it is visible but not active. \l clicked
        signal is not emitted.
  \row
    \li Active
    \li The hint is visible and active, clicking on it with the mouse
        triggers the \l clicked signal. When set, visuals should be able to
        transition into \e Idle state.
  \row
    \li Locked
    \li The hint is locked into the \e Active state and it is expected that
        visuals cannot transition back to any other state.
  \endtable

  Defaults to:
  \list
    \li \e Idle when no mouse is attached
    \li \e Locked when mouse is attached
  \endlist
 */
