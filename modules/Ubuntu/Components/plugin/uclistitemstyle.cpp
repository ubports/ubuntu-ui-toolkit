/*
 * Copyright 2014-2015 Canonical Ltd.
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

#include "uclistitemstyle.h"
#include "i18n.h"
#include "uclistitem.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickanimation_p.h>

/*!
 * \qmltype ListItemStyle
 * \instantiates UCListItemStyle
 * \inqmlmodule Ubuntu.Components.Styles 1.2
 * \ingroup style-api
 * \since Ubuntu.Components.Styles 1.2
 * \brief Style API for ListItem component.
 *
 * Style API for the ListItem component which provides actions, select and
 * drag handler delegates, and snap animation via its properties.
 * ListItem treats the style differently compared to the other components,
 * as it loads the style only when needed and not upon component creation.
 */
UCListItemStyle::UCListItemStyle(QQuickItem *parent)
    : QQuickItem(parent)
    , m_snapAnimation(0)
    , m_selectionDelegate(0)
    , m_animatePanels(true)
{
}

void UCListItemStyle::componentComplete()
{
    QQuickItem::componentComplete();

    // look for overridden slots
    for (int i = metaObject()->methodOffset(); i < metaObject()->methodCount(); i++) {
        const QMetaMethod method = metaObject()->method(i);
        if (method.name() == QByteArrayLiteral("swipeEvent")) {
            m_swipeEvent = method;
        } else if (method.name() == QByteArrayLiteral("rebound")) {
            m_rebound = method;
        }
    }

    // connect snapAnimation's stopped() and the owning ListItem's sontentMovementeEnded() signals
    UCListItem *listItem = qmlContext(this)->contextProperty("styledItem").value<UCListItem*>();
    if (listItem && m_snapAnimation) {
        connect(m_snapAnimation, SIGNAL(runningChanged(bool)),
                listItem, SLOT(_q_contentMoving()));
    }
}

/*!
 * \qmlmethod ListItemStyle::swipeEvent(SwipeEvent event)
 * The function is called by the ListItem when a swipe action is performed, i.e.
 * when the swipe is started, the position is updated or the swipe ends. The
 * \b event object provides information about the swipe status, positions and
 * the updated \l {ListItem::contentItem}{ListItem.contentItem} position. The
 * style implementation can override the contentItem position by setting the
 * \c event.content.x or \c event.content.y properties to the desired value.
 *
 * The \c event object properties are:
 * \list
 * \li * \c status - enumeration of \c {Started, Updated, Finished} values representing
 *                  the swipe event status
 * \li * \c mouse - (x, y) coordinates of the current mouse/touch point - read-only
 * \li * \c last - (x, y) coordinates of the previous mouse/touch point - read-only
 * \li * \c content - (x, y) updated coordinates of the \l {ListItem::contentItem}
 *                  {ListItem.contentItem}, read-write
 * \endlist
 */
void UCListItemStyle::swipeEvent(UCSwipeEvent *event)
{
    Q_UNUSED(event);
    qmlInfo(this) << UbuntuI18n::instance().tr("consider overriding swipeEvent() slot!");
}
void UCListItemStyle::invokeSwipeEvent(UCSwipeEvent *event)
{
    if (m_swipeEvent.isValid()) {
        m_swipeEvent.invoke(this, Q_ARG(QVariant, QVariant::fromValue(event)));
    } else {
        swipeEvent(event);
    }
}

/*!
 * \qmlmethod ListItemStyle::rebound()
 * Function called by the ListItem when a rebounding action is requested from the
 * style. This usually happens when the list item's content is swiped and there is
 * a press event happening outside of the ListItem's boundary or when the view
 * embedding the ListItem starts scrolling.
 */
void UCListItemStyle::rebound()
{
    qmlInfo(this) << UbuntuI18n::instance().tr("consider overriding rebound() slot!");
}
void UCListItemStyle::invokeRebound()
{
    if (m_rebound.isValid()) {
        m_rebound.invoke(this);
    } else {
        rebound();
    }
}

/*!
 * \qmlproperty Animation ListItemStyle::snapAnimation
 * Holds the behavior used in animating when snapped in or out.
 */

/*!
 * \qmlproperty Component ListItemStyle::selectionDelegate
 * Holds the component handling the selection mode. The component is responsible
 * to handle the visualization for the selection mode, updating the visuals of
 * the ListItem (e.g. pushing the \l {ListItem::contentItem}{contentItem},
 * animating the changes, resizing the contentItem, etc.), as well as reporting
 * the changes to the ListItem whenever the selection is changed.
 *
 * ListItem will create the component when the selection mode is entered, and will
 * keep it for the entire lifetime of the ListItem, even if the selection mode is
 * exited. Therefore implementations must take care of hiding the visuals when
 * leaving selection mode.
 *
 * The parent of the component is the ListItem itself, and the ListItem attached
 * object will be also attached to it.
 *
 * The \l ListItem::animate attached property is set if the ListItem expects to animate
 * the panel changes, and reset if should not.
 *
 * \note If states are used to show/hide and animate the panel changes, these changes
 * should be applied only after the component completion happens, otherwise animations
 * may not be executed during the first creation time.
 * \qml
 * ListItemStyle {
 *     // [...]
 *     selectionDelegate: Item {
 *         id: panel
 *         width: units.gu(5)
 *         anchors {
 *             top: parent ? parent.top : undefined
 *             right: parent ? parent.left : undefined
 *             bottom: parent ? parent.bottom : undefined
 *         }
 *         states: State {
 *             name: "enabled"
 *             PropertyChanges {
 *                 // [...]
 *             }
 *         }
 *         transitions: Transitions {
 *             from: ""
 *             to: "enabled"
 *             enabled: panel.ListItem.animate
 *             // [...]
 *         }
 *
 *         // other content
 *         // [...]
 *
 *         // do a function binding on state now
 *         readonly property ListItem listItem: parent
 *         Component.onCompleted: {
 *             state = Qt.binding(function () {
 *                 return listItem && listItem.selectable ? "enabled" : "";
 *             });
 *         }
 *     }
 * }
 * \endqml
 */

