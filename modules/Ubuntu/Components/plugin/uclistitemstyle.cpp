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
 */

#include "uclistitemstyle.h"

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
 * as it:
 * \list
 *  \li - loads the style only when needed
 *  \li - gets delegates and snap animation from the style properties
 *  \li - ignores any other visuals defined in the style.
 * \endlist
 */
UCListItemStyle::UCListItemStyle(QQuickItem *parent)
    : QQuickItem(parent)
    , m_actionsDelegate(0)
    , m_selectionDelegate(0)
    , m_dragHandlerDelegate(0)
    , m_snapAnimation(0)
    , m_swipeOvershoot(0)
{
}

/*!
 * \qmlproperty Component ListItemStyle::actionsDelegate
 * Specifies the component visualizing the leading/trailing actions.
 */

/*!
 * \qmlproperty Component ListItemStyle::selectionDelegate
 * Holds the component handling the selection mode. The component is responsible
 * to handle the visualization for the selection mode, updating the visuals of
 * the ListItem (e.g. pushing the \l {ListItem::contentItem}{contentItem},
 * animating the changes, resizing the contentItem, etc.), as well as reporting
 * the changes to the ListItem whenever the selection is changed.
 *
 * ListItem will create the component when the selection mode is emntered, and will
 * keep it for the entire lifetime of the ListItem, even if the selection mode is
 * exited. Therefore implementations must take care of removing the visuals when
 * leaving selection mode.
 *
 * The parent of the component is the ListItem itself.
 *
 * The \c animatePanel context property is set if the ListItem expects to animate
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
 *             enabled: animatePanel
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

/*!
 * \qmlproperty Component ListItemStyle::dragHandlerDelegate
 * Holds the component shown when dragging mode is enabled. The component does
 * not need to handle the mouse/touch events to detect when the handler is dragged/
 * dropped. The handler being parented to ListItem, all its properties can be reached,
 * as well as all its context properties (such as \c index) and model roles in case
 * used in a modelled view. An additional context proeprty is also declared to notify
 * when the handler can be enabled, the \c draggingEnabled context property.
 *
 * The \c animatePanel context property is set if the ListItem expects to animate
 * the panel changes, and reset if should not.
 *
 * \note The drag handler is only instantiated if the dragging mode is possible,
 * meaning the ListItem is used in a ListView. Therefore entering in dragging mode
 * can also be checked by accessing the \l ListItem::draggable property through \c
 * ListView.view property. Example:
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 * import Ubuntu.Components.Styles 1.2 as Styles
 *
 * Styles.ListItemStyle {
 *     // [...]
 *     dragHandlerDelegate: Rectangle {
 *         id: dragHandler
 *         width: height
 *         // Internally used to link to the list item's content. The parent item is the ListItem itself.
 *         readonly property Item contentItem: parent ? parent.contentItem : null
 *
 *         // track drag mode status
 *         readonly property bool draggingEnabled: parent && parent.ListView.view ?
 *                                                   parent.ListView.view.ListItem.draggable : false
 *
 *         anchors {
 *             // by default the panel stays outside of the ListItem's right side
 *             left: parent ? parent.right : undefined
 *             top: contentItem ? contentItem.top : undefined
 *             bottom: contentItem ? contentItem.bottom : undefined
 *         }
 *
 *         Icon {
 *             objectName: "icon"
 *             id: dragIcon
 *             anchors.centerIn: parent
 *             width: units.gu(2.5)
 *             height: width
 *             name: "view-grid-symbolic"
 *         }
 *
 *         states: State {
 *             name: "enabled"
 *             AnchorChanges {
 *                 target: dragHandler
 *                 anchors.right: dragHandler.parent.right
 *                 anchors.left: undefined
 *             }
 *         }
 *
 *         transitions: Transition {
 *             from: ""
 *             to: "enabled"
 *             enabled: animatePanel
 *             reversible: true
 *             AnchorAnimation {
 *                 easing: UbuntuAnimation.StandardEasing
 *                 duration: UbuntuAnimation.FastDuration
 *             }
 *         }
 *
 *         // do a function binding on state now to make sure we animate when we have to
 *         readonly property ListItem listItem: parent
 *         Component.onCompleted: {
 *             state = Qt.binding(function () {
 *                 return listItem && listItem.draggable ? "enabled" : "";
 *             });
 *         }
 *    }
 * }
 * \endqml
 */

/*!
 * \qmlproperty PropertyAnimation ListItemStyle::snapAnimation
 * Holds the animation used in animating when snapped in or out.
 */

/*!
 * \qmlproperty real ListItemStyle::swipeOvershoot
 * The property specifies the overshoot value of the ListItem.
 */
