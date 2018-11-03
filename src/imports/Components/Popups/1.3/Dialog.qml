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

import QtQuick 2.4
import QtQuick.Window 2.2
import Ubuntu.Components 1.3
import "internalPopupUtils.js" as InternalPopupUtils

/*!
    \qmltype Dialog
    \inherits PopupBase
    \inqmlmodule Ubuntu.Components.Popups 1.0
    \ingroup ubuntu-popups
    \brief The Dialog caters for cases in which the application requires the user to determine
        between optional actions. The Dialog will interrupt the user flow and lock the view
        for further interaction before the user has selected a desired action.
        It can only be closed by selecting an optional action confirming or cancelling the operation.

    \l {http://design.ubuntu.com/apps/building-blocks/dialog}{See also the Design Guidelines on Dialogs}.

    Example:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        import Ubuntu.Components.Popups 1.3

        Item {
            width: units.gu(80)
            height: units.gu(80)
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
                         color: UbuntuColors.orange
                         onClicked: PopupUtils.close(dialogue)
                     }
                     Button {
                         text: "save a copy"
                         color: UbuntuColors.orange
                         onClicked: PopupUtils.close(dialogue)
                     }
                 }
            }
            Button {
                anchors.centerIn: parent
                id: saveButton
                text: "save"
                onClicked: PopupUtils.open(dialog)
            }
        }
    \endqml
*/

PopupBase {
    id: dialog

    /*!
      \qmlproperty list<Object> contents
      Content will be put inside a column in the foreround of the Dialog.
    */
    default property alias contents: contentsColumn.data

    /*!
      The title of the question to ask the user.
      \qmlproperty string title
     */
    property alias title: foreground.title

    /*!
      The question to the user.
      \qmlproperty string text
     */
    property alias text: foreground.text

    /*!
      \deprecated
      The Item such as a \l Button that the user interacted with to open the Dialog.
      This property will be used for the automatic positioning of the Dialog next to
      the caller, if possible.
     */
    property Item caller
    onCallerChanged: console.warn("'caller' is DEPRECATED. It has no effect.")

    /*!
      The property holds the margins from the dialog's dismissArea. The property
      is themed.
      */
    property real edgeMargins: units.gu(2)

    /*!
      The property holds the margin from the dialog's caller. The property
      is themed.
      */
    property real callerMargin: units.gu(1)

    /*!
      The property controls whether the dialog is modal or not. Modal dialogs block
      event propagation to items under dismissArea, when non-modal ones let these
      events passed to these items. In addition, non-modal dialogs do not dim the
      dismissArea.

      The default value is true.
      */
    property bool modal: true

    /*!
      \qmlproperty real contentWidth
      \since Ubuntu.Components 1.3
      Specifies the width of the dialog content.
      */
    property alias contentWidth: foreground.width

    /*!
      \qmlproperty real contentHeight
      \since Ubuntu.Components 1.3
      Specifies the height of the dialog content.
      */
    property alias contentHeight: foreground.height

    /*!
      \qmlproperty ThemeSettings theme
      \since Ubuntu.Components 1.3
      Configure the theme of the Dialog foreground and all its subcomponents.
      Example:
      \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        import Ubuntu.Components.Popups 1.3
        MainView {
            width: units.gu(40)
            height: units.gu(71)

            // make sure the main theme is Ambiance
            theme.name: "Ubuntu.Components.Themes.Ambiance"

            Component {
                id: dialogComponent
                Dialog {
                    id: dialog
                    title: "Input dialog"
                    // the dialog and its children will use SuruDark
                    theme: ThemeSettings {
                        name: "Ubuntu.Components.Themes.SuruDark"
                    }
                    TextField {
                        placeholderText: "enter text"
                    }
                    Button {
                        text: "Close"
                        onClicked: PopupUtils.close(dialog)
                    }
                }
            }

            Button {
                text: "Open dialog"
                onClicked: PopupUtils.open(dialogComponent)
            }
        }
      \endqml
      \sa StyledItem::theme
     */
    property alias theme: foreground.theme

    /*!
      \qmlproperty string styleName
      The style name of the foreground of the Dialog.
      \since Ubuntu.Components 1.3
      \sa StyledItem::styleName
     */
    property alias styleName: foreground.styleName

    /*!
      \qmlproperty Component style
      The style of the foreground of the Dialog.
      This property takes precedence over \l styleName
      \since Ubuntu.Components 1.3
      \sa StyledItem::style
     */
    property alias style: foreground.style

    __foreground: foreground
    __eventGrabber.enabled: modal
    __dimBackground: modal
    fadingAnimation: UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }

    StyledItem {
        id: foreground
        // Grab focus when Dialog is shown
        focus: visible
        width: Math.min(minimumWidth, dialog.width)
        anchors.centerIn: parent
        clip: true
        objectName: 'dialogForeground'

        // used in the style
        property string title
        property string text
        property real minimumWidth: units.gu(38)
        property real minimumHeight: units.gu(32)
        property real maxHeight: 3*dialog.height/4
        property real margins: units.gu(4)
        property real itemSpacing: units.gu(2)
        property Item dismissArea: dialog.dismissArea
        property real keyboardHeight: dialog.anchorToKeyboard && UbuntuApplication.inputMethod.visible ? UbuntuApplication.inputMethod.keyboardRectangle.height / Screen.devicePixelRatio : 0

        height: Math.min(contentsColumn.height + foreground.margins, dialog.height - keyboardHeight)

        Keys.onEscapePressed: dialog.hide()

        Flickable {
            anchors.fill: parent
            anchors.margins: foreground.margins
            contentWidth: contentsColumn.width
            contentHeight: contentsColumn.height - foreground.margins
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: contentsColumn
                spacing: foreground.itemSpacing
                width: foreground.width - foreground.margins * 2
                height: childrenRect.height + foreground.margins
                onWidthChanged: updateChildrenWidths();

                // put the context into this component to save ActionContext lookup
                PopupContext {
                    id: localContext
                    objectName: dialog.objectName + "DialogContext"
                    active: foreground.visible
                }

                Label {
                    horizontalAlignment: Text.AlignHCenter
                    text: dialog.title
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                    elide: Text.ElideRight
                    textSize: Label.Large
                    color: theme.palette.normal.overlayText
                    visible: (text !== "")
                }

                Label {
                    horizontalAlignment: Text.AlignHCenter
                    text: dialog.text
                    color: theme.palette.normal.overlayText
                    wrapMode: Text.Wrap
                    visible: (text !== "")
                }

                onChildrenChanged: updateChildrenWidths()

                function updateChildrenWidths() {
                    for (var i = 0; i < children.length; i++) {
                        children[i].width = contentsColumn.width;
                    }
                }
            }
        }

        styleName: "DialogForegroundStyle"

        Component.onCompleted: foreground.forceActiveFocus()
    }
}
