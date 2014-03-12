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

import QtQuick 2.0
import Ubuntu.Components.Popups 0.1
import "mathUtils.js" as MathUtils

/*!
    \qmltype ComboButton
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Ubuntu button providing a drop-down panel visualizing custom options.

    The component is composed of three main blocks: \b {main button}, \b {dropdown}
    and \b {combo list}.

    The \a {main button} holds the main functionailty of the component, and it is
    located at the left-top side of the expanded button. The \a clicked() signal
    is triggered only when this button is pressed.

    The \a {dropdown} is a button located on the top-right side of the expanded
    component. Its functionality is to drive the component's expanded state.

    The \a {combo list} is a panel showing the content in the expanded state. The
    content can be any component with any size, however cropped in the height
    given in the \l expandedHeight property. If the content height exceeds this
    value, the content will be scrolled. Also, if the content height is less than
    the value in the \l expandedHeight, the combo list will not be expanded to the
    full expandedHeight.

    \note Do not set the component's height directly, as expansion controls its value
    and overriding it may destroy component's proper functioning. Instead use
    \l collapsedHeight and \l expandedHeight to control the height in collapsed and
    expanded states.

    The combo list has a built-in Flickable which provides you the ability to scroll
    the content when that exceeds the height provided in \l expandedHeight. If the
    content of the combo list is declared as a single Flickable, ListView, GridView
    or PathView component, the content list scrolling will be disabled allowing the
    provided component to do the scrolling. In this case it is recommended to set
    the height of the component to \l comboListHeight avoiding unwanted clipping
    in case the size exceeds the maximum allowed one.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    ComboButton {
        text: "ListView"
        ListView {
            width: parent.width
            height: comboListHeight
            model: 20
            delegate: Standard {
                text: "Action #" + modelData
            }
        }
    }
    \endqml

    An other way to control the expansion is to make sure the component expands
    as much as the combo list content does. In this case the \l expandedHeight
    should be set to -1.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    ComboButton {
        text: "auto expandHeight"
        expandedHeight: -1
        Column {
            id: column
            anchors {
                left: parent.left
                right: parent.right
            }
            Rectangle {
                width: parent.width
                height: units.gu(6)
                color: "red"
            }
            Rectangle {
                width: parent.width
                height: units.gu(6)
                color: "green"
            }
            Rectangle {
                width: parent.width
                height: units.gu(6)
                color: "blue"
            }
            Rectangle {
                width: parent.width
                height: units.gu(6)
                color: "yellow"
            }
        }
    }
    \endqml

    As combo list content is unknown for the component, eventual collapsing when
    an action selected from the combo list holder must be driven manually. Also,
    the default action of the main button can be changed in this way.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    ComboButton {
        id: combo
        text: "Auto closing"
        expanded: true
        expandedHeight: units.gu(30)
        UbuntuListView {
            width: parent.width
            height: combo.comboListHeight
            model: 20
            delegate: Standard {
                text: "Action #" + modelData
                onClicked: {
                    combo.text = text;
                    combo.expanded = false;
                }
            }
        }
    }
    \endqml

    The style of the component is defined in \l ComboButtonStyle.
  */
Button {
    id: combo

    /*!
      Specifies whether the combo list is expanded or not. The default falue is
      false.
      */
    property bool expanded: false

    /*!
      The property specifies whether to collapse the expanded combo list when
      the main button is clicked. The default value is true.
      */
    property bool autoCollapse: true

    /*!
      The property holds the height of the component when collapsed. By default
      the value is the implicit height of the component.
      */
    property real collapsedHeight: implicitHeight

    /*!
        The property holds the maximum height value the component should expand.
        When setting the property, remember to take into account the \l collapsedHeight
        value. The best practice is to use bind it with collapsedHeight.
        \qml
        ComboButton {
            text: "altered expandedHeight"
            expandedHeight: collapsedHeight + units.gu(25)
        }
        \endqml

        A value of -1 will instruct the component to expand the combo list as
        much as its content height is.

        The default value is \l collapsedHeight + 15 GU.
        \sa collapsedHeight
      */
    property real expandedHeight: collapsedHeight + units.gu(15)

    /*!
      The property holds the maximum combo list height allowed based on the
      \l expandedHeight and \l collapsedHeight values. It is a convenience property
      that can be used to size the combo list content.
      \qml
      import QtQuick 2.0
      import Ubuntu.Components 0.1
      import Ubuntu.Components.ListItems 0.1
      ComboButton {
          id: combo
          text: "Full comboList size"
          ListView {
              anchors {
                  left: parent.left
                  right: parent.right
              }
              height: combo.comboListHeight
              model: 20
              delegate: Standard {
                  text: "Action #" + modelData
              }
          }
      }
      \endqml

      \sa collapsedHeight, expandedHeight
      */
    readonly property real comboListHeight: (expandedHeight < 0) ?
                            comboListHolder.height :
                            (expandedHeight - collapsedHeight -
                            (combo.__styleInstance ? combo.__styleInstance.comboListMargin : 0))

    /*!
        \qmlproperty list<Item> comboList
        \default
        Default property holding the item to be shown in the combo list.
      */
    default property alias comboList: comboListHolder.data

    style: Theme.createStyleComponent("ComboButtonStyle.qml", combo)

    // auto-collapse
    /*! \internal */
    onClicked: {
        if (autoCollapse) {
            expanded = false;
        }
    }

    Component.onCompleted: {
        // update mouse area to report clicks only on the main button area
        // area excluding dropDown button and combo list
        // we must do separate bindings as __mouseArea is a read-only property
        __mouseArea.anchors.fill = undefined;
        __mouseArea.anchors.left = Qt.binding(function() {return combo.left;});
        __mouseArea.anchors.top = Qt.binding(function() {return combo.top;});
        __mouseArea.anchors.right = Qt.binding(function() {return combo.right;});
        __mouseArea.anchors.rightMargin = Qt.binding(function() {return combo.__styleInstance.dropDownWidth + combo.__styleInstance.dropDownSeparatorWidth;});
        __mouseArea.height = Qt.binding(function() {return collapsedHeight;});

        // bind a height calculation to avoid unwanted change
        combo.height = Qt.binding(function() {return collapsedHeight + __styleInstance.comboListPanel.height});
    }

    // dropdown button
    MouseArea {
        id: dropDown
        objectName: "combobutton_dropdown"
        anchors {
            right: parent.right
            top: parent.top
        }
        width: combo.__styleInstance ? combo.__styleInstance.dropDownWidth : 0
        height: combo.collapsedHeight
        // open dropdown when pressed, not when clicked
        onPressed: {
            // toggle expanded
            combo.expanded = !combo.expanded;
        }
    }

    // expansion list
    Flickable {
        id: comboHolder
        parent: combo.__styleInstance ? combo.__styleInstance.comboList : combo
        anchors.fill: parent
        interactive: combo.expanded && !contentIsFlickable() && (combo.expandedHeight > 0)
        flickableDirection: Flickable.VerticalFlick
        contentHeight: interactive ? comboListHolder.height : parent.height

        // consider PathView as Flickable in this case as well!
        function contentIsFlickable() {
            return (comboListHolder.children.length === 1) &&
                    comboListHolder.children[0].hasOwnProperty("flicking");
        }

        Item {
            id: comboListHolder
            objectName: "combobutton_combolist"
            anchors {
                left: parent.left
                right: parent.right
            }
            height: childrenRect.height
        }
    }
    Scrollbar {
        flickableItem: comboHolder
    }

    // drive the expansion height of the combo list holder
    Binding {
        target: combo.__styleInstance.comboListPanel
        property: "height"
        value: {
            if (expanded) {
                var h = comboListHolder.height;
                var max = combo.comboListHeight + combo.__styleInstance.comboListMargin;
                if (combo.expandedHeight < 0) {
                    return max;
                }

                return MathUtils.clamp(h, 0, max);
            }
            return 0;
        }
    }
    // for testing purposes only
    Binding {
        target: combo.__styleInstance.comboListPanel
        property: "objectName"
        value: "combobutton_combopanel"
    }
}
