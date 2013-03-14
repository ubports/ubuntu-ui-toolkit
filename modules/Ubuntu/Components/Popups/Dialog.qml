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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming
import "internalPopupUtils.js" as InternalPopupUtils

/*!
    \qmltype Dialog
    \inherits PopupBase
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

    /*!
      \preliminary
      Content will be put inside a column in the foreround of the Dialog.
    */
    default property alias contents: contentsColumn.data

    /*!
      \preliminary
      The title of the question to ask the user.
      \qmlproperty string title
     */
    property alias title: foreground.title

    /*!
      \preliminary
      The question to the user.
      \qmlproperty string text
     */
    property alias text: foreground.text

    /*!
      \preliminary
      The Item such as a \l Button that the user interacted with to open the Dialog.
      This property will be used for the automatic positioning of the Dialog next to
      the caller, if possible.
     */
    property Item caller

    /*!
      The property holds the item to which the pointer should be anchored to.
      This can be same as the caller or any child of the caller. By default the
      property is set to caller.
      */
    property Item pointerTarget
    /*! \internal */
    onPointerTargetChanged: {
        console.debug("pointerTarget DEPRECATED")
    }

    /*!
      The property holds the margins from the dialog's dismissArea. The property
      is themed.
      */
    property real edgeMargins

    /*!
      The property holds the margin from the dialog's caller. The property
      is themed.
      */
    property real callerMargin

    /*!
      The property controls whether the dialog is modal or not. Modal dialogs block
      event propagation to items under dismissArea, when non-modal ones let these
      events passed to these items. In addition, non-modal dialogs do not dim the
      dismissArea.

      The default value is true.
      */
    property bool modal: true

    Theming.ItemStyle.class: "dialog"

    /*
    QtObject {
        id: internal

        function updatePosition() {
            var pos = new InternalPopupUtils.CallerPositioning(foreground, pointer, dialog, caller, pointerTarget, edgeMargins, callerMargin);
            pos.auto();

        }
    }

    Pointer { id: pointer }
    */

    __foreground: foreground
    __eventGrabber.enabled: modal
    __dimBackground: modal

    Item {
        id: foreground
        // FIXME: see above
        Theming.ItemStyle.class: "foreground"
        width: Math.min(minimumWidth, dialog.width)
        anchors.centerIn: parent

        // used in the delegate
        property string title
        property string text
        property real minimumWidth
        property real minimumHeight
        property real maxHeight: 3*dialog.height/4
        property real margins
        property real itemSpacing

        height: childrenRect.height

        Column {
            id: contentsColumn
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: foreground.margins
            }
            spacing: foreground.itemSpacing
            height: childrenRect.height + foreground.margins
            onWidthChanged: updateChildrenWidths();

            Label {
                ItemStyle.class: "title"
                horizontalAlignment: Text.AlignHCenter
                text: dialog.title
            }

            Label {
                horizontalAlignment: Text.AlignHCenter
                text: dialog.text
            }

            onChildrenChanged: updateChildrenWidths()

            function updateChildrenWidths() {
                for (var i = 0; i < children.length; i++) {
                    children[i].width = contentsColumn.width;
                }
            }
        }
    }
}
