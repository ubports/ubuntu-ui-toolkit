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
import "internalPopupUtils.js" as InternalPopupUtils
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype Popover
    \inherits PopupBase
    \inqmlmodule Ubuntu.Components.Popups 0.1
    \ingroup ubuntu-popups
    \brief A popover allows an application to present additional content without changing the view.
        A popover has a fixed width and automatic height, depending on is contents.
        It can be closed by clicking anywhere outside of the popover area.

    Example:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.Popups 0.1

        Item {
            Popover {
                id: popover
                Column {
                    id: containerLayout
                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }
                    ListItem.Header { text: "Standard list items" }
                    ListItem.Standard { text: "Do something" }
                    ListItem.Standard { text: "Do something else" }
                    ListItem.Header { text: "Buttons" }
                    ListItem.SingleControl {
                        highlightWhenPressed: false
                        control: Button {
                            text: "Do nothing"
                            anchors {
                                fill: parent
                                margins: units.gu(1)
                            }
                        }
                    }
                    ListItem.SingleControl {
                        highlightWhenPressed: false
                        control: Button {
                            text: "Close"
                            anchors {
                                fill: parent
                                margins: units.gu(1)
                            }
                            onClicked: PopupUtils.close(popover)
                        }
                    }
                }
            }
            Button {
                id: popoverButton
                text: "open"
                onClicked: PopupUtils.open(popoverComponent, popoverButton)
            }
        }
    \endqml
*/
PopupBase {
    id: popover

    /*!
      Content will be put inside the foreround of the Popover.
    */
    default property alias container: containerItem.data

    /*!
      \qmlproperty real contentWidth
      \qmlproperty real contentHeight
      The properties can be used to alter the default content width and heights.
      */
    property alias contentWidth: foreground.width
    /*! \internal */
    property alias contentHeight: foreground.height

    /*!
      The Item such as a \l Button that the user interacted with to open the Dialog.
      This property will be used for the automatic positioning of the Dialog next to
      the caller, if possible.
     */
    property Item caller

    QtObject {
        id: internal
        // TODO: put the margins in the style
        property real edgeMargins: units.gu(2)
        property real callerMargins: units.gu(1)
        property bool portrait: width < height

        // private
        function updatePosition() {
            var pos = new InternalPopupUtils.CallerPositioning(foreground, pointer, dismissArea, caller, edgeMargins, callerMargins);
            pos.auto();
        }
    }

    Theming.InverseMouseArea {
        anchors.fill: foreground
        sensingArea: dismissArea
        propagateComposedEvents: !grabDismissAreaEvents
        onPressed: popover.hide()
    }

    Item {
        id: foreground

        // FIXME: see above
        Theming.ItemStyle.class: "popover"

        property real maxWidth: dismissArea ? (internal.portrait ? dismissArea.width : dismissArea.width * 3/4) : 0.0
        property real maxHeight: dismissArea ? (internal.portrait ? dismissArea.height * 3/4 : dismissArea.height) : 0.0
        width: Math.min(ComponentUtils.style(foreground, "minimumWidth", units.gu(40)), maxWidth)
        height: childrenRect.height

        Item {
            id: containerItem
            anchors {
                left: parent.left
                top: parent.top
                right: parent.right
            }
            height: childrenRect.height
        }

        onWidthChanged: internal.updatePosition()
        onHeightChanged: internal.updatePosition()

        // Avoid mouse events being sent to any MouseAreas that are behind the popover
        MouseArea {
            anchors.fill: parent
            z: -1
        }
    }

    Pointer {
        id: pointer
        longAxis: 2*internal.callerMargins
        shortAxis: internal.callerMargins
    }

    /*! \internal */
    onCallerChanged: internal.updatePosition()

    /*! \internal */
    onWidthChanged: internal.updatePosition()

    /*! \internal */
    onHeightChanged: internal.updatePosition()
}
