/*
 * Copyright 2012 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components 0.1 as Theming
import "internalPopupUtils.js" as InternalPopupUtils

/*!
    \qmltype Dialog
    \inqmlmodule Ubuntu.Components.Popups 0.1
    \ingroup ubuntu-popups
    \brief The Dialog caters for cases in which the application requires the user to determine
        between optional actions. The Dialog will interrupt the user flow and lock the view
        for further interaction before the user has selected a desired action.
        It can only be closed by selecting an optional action confirming or cancelling the operation.

    Example:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.Popups 0.1

        Item {
            Component {
                 id: dialog
                 Dialog {
                     id: dialogue
                     title: "Save file"
                     text: "Are you sure that you want to save this file?"
                     Button {
                         text: "cancel"
                         onClicked: PopupUtils.close(dialogue)
                     }
                     Button {
                         text: "overwrite previous version"
                         color: "orange"
                         onClicked: PopupUtils.close(dialogue)
                     }
                     Button {
                         text: "save a copy"
                         color: "orange"
                         onClicked: PopupUtils.close(dialogue)
                     }
                 }
            }
            Button {
                anchors.centerIn: parent
                id: saveButton
                text: "save"
                onClicked: PopupUtils.open(dialog, saveButton)
            }
        }
    \endqml
*/

PopupBase {
    id: dialog

    /*! \internal */
    default property alias __buttons: buttonColumn.data

    /*!
      \preliminary
      The title of the question to ask the user.
      \qmlproperty string title
     */
    property alias title: foreground.title //headerText.text

    /*!
      \preliminary
      The question to the user.
      \qmlproperty string text
     */
    property alias text: foreground.text //questionText.text

    /*!
      \preliminary
      The Item such as a \l Button that the user interacted with to open the Dialog.
      This property will be used for the automatic positioning of the Dialog next to
      the caller, if possible.
     */
    property Item caller

    QtObject {
        id: internal

        // TODO: Move the two properties below, and various margins and colors, to a delegate.
        property real edgeMargins: units.gu(2)
        property real callerMargins: units.gu(2)

        function updatePosition() {
            var pos = new InternalPopupUtils.CallerPositioning(foreground, pointer, dialog, caller, edgeMargins, callerMargins);
            pos.auto();
        }
    }

    Background {
        dim: true
        ephemeral: false
    }

    Pointer {
        id: pointer
        color: "grey"
        opacity: 0.9
        longAxis: 2*internal.callerMargins
        shortAxis: internal.callerMargins
    }

    Foreground {
        id: foreground

        color: "green"

        Theming.ItemStyle.class: "dialog-foreground"
        width: Math.min(units.gu(40), dialog.width)
        property string maxHeight: 3*dialog.height/4
        height: childrenRect.height //MathUtils.clamp(childrenRect.height, units.gu(32), 3*dialog.height/4)

        property string title
        property string text

        Column {
            id: buttonColumn
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(1)

            height: childrenRect.height + anchors.bottomMargin

            onChildrenChanged: {
                for (var i = 0; i < children.length; i++) {
                    children[i].anchors.left = buttonColumn.left;
                    children[i].anchors.right = buttonColumn.right;
                }
            }
        }

        onWidthChanged: internal.updatePosition()
        onHeightChanged: internal.updatePosition()
    }

    /*! \internal */
    onCallerChanged: internal.updatePosition()

    /*! \internal */
    onWidthChanged: internal.updatePosition()

    /*! \internal */
    onHeightChanged: internal.updatePosition()
}
