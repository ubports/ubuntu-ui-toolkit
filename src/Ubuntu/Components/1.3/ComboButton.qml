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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

/*!
    \qmltype ComboButton
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief Ubuntu button providing a drop-down panel visualizing custom options.

    The component is composed of three main blocks: \b {main button}, \b {dropdown}
    and \b {combo list}.

    The \a {main button} holds the main functionailty of the component, and it is
    located at the left-top side of the expanded button. The \a clicked() signal
    is triggered only when this button is pressed.

    The \a {dropdown} is a button located on the right of the main button. Its
    functionality is to drive the component's expanded state.

    The \a {combo list} is a panel showing the content specified in \l comboList
    property when expanded. The content is stretched horizontally to the component's
    width, and its height is controlled by the \l expandedHeight property as follows:
    \list
    \li If the content height is smaller than the value of \l expandedHeight, the combo
        list will be expanded only to the height of the content.
        \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        ComboButton {
            text: "smaller content"
            Rectangle {
                height: units.gu(5) // smaller than the default expandedHeight
                color: "blue"
            }
        }
        \endqml
    \li If the content height is greater than \l expandedHeight, the combo list will
        expand till the height specified by the property and the content will be
        scrolled; in case the combo list content is one single Flickable, ListView,
        GridView or PathView, the content scrolling will be provided by the content
        itself.
        \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        ComboButton {
            text: "long scrolled content"
            Column {
                Repeater {
                    model: 5
                    spacing: units.gu(1)
                    Rectangle {
                        height: units.gu(5)
                        color: "blue"
                    }
                }
            }
        }
        \endqml
    \li In case the content is a single Flickable, ListView, GridView or PathView,
        the content will be filling the entire combo list area defined.
        \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        import Ubuntu.Components.ListItems 1.3
        ComboButton {
            text: "listview"
            ListView {
                model: 10
                delegate: Standard {
                    text: "Item #" + modelData
                }
            }
        }
        \endqml
    \li Vertical anchoring of combo list content to its parent is not possible
        as the expansion calculation is done based on the combo list content height.
        If the content wants to take the size of the entire combo list, it should
        bind its height to the \l comboListHeight property.
        \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        ComboButton {
            id: combo
            text: "smaller content"
            Rectangle {
                height: combo.comboListHeight
                color: "blue"
            }
        }
        \endqml
    \li In case the expansion needs to be the size of the combo list content, the \l
        expandedHeight should be set to -1.
        \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        ComboButton {
            text: "auto-sized content"
            expandedHeight: -1
            Column {
                Repeater {
                    model: 5
                    spacing: units.gu(1)
                    Button {
                        text: "Button #" + modelData
                    }
                }
            }
        }
        \endqml
    \endlist

    The combo list can be expanded/collapsed either through the \l expanded property
    or by clicking on the dropdown button. It is not collapsed when pressing the main
    button or clicking on the combo list. In order to do an auto-collapsing button
    you must reset the expanded property (set it to false) when the main button is
    clicked or when a selection is taken from the combo list content. The following
    example illustrates a possible implementation.

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    ComboButton {
        id: combo
        text: "Auto closing"
        expanded: true
        expandedHeight: units.gu(30)
        onClicked: expanded = false
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

    \section2 Styling
    The style of the component is defined in \l ComboButtonStyle.
  */
AbstractButton {
    id: combo

    /*!
      \deprecated
      If set to a color, the button has a stroke border instead of a filled
      shape.
    */
    property color strokeColor
    onStrokeColorChanged: console.warn(
        "WARNING: ComboButton.strokeColor is deprecated."
    )

    /*!
      \deprecated
      The background color of the button.
    */
    property color color
    onColorChanged: console.warn("WARNING: ComboButton.color is deprecated.")

    /*!
      \deprecated
      The gradient used to fill the background of the button.
      Standard Ubuntu gradients are defined in \l UbuntuColors.
      If both a gradient and a color are specified, the gradient will be used.
    */
    property Gradient gradient
    onGradientChanged: console.warn(
        "WARNING: ComboButton.gradient is deprecated."
    )

    /*!
      \deprecated
      The property specifies the color of the dropdown button and the combo
      list for both collapsed and expanded states. You can use \l expanded to
      define different colors for expanded or collapsed states.
      */
    property color dropdownColor
    onDropdownColorChanged: console.warn(
        "WARNING: ComboButton.dropdownColor is deprecated."
    )

    /*!
      The font used for the button's text.
    */
    property font font: __styleInstance.defaultFont

    /*!
      The position of the icon relative to the text. Options
      are "left" and "right". The default value is "left".

      If only text or only an icon is defined, this
      property is ignored and the text or icon is
      centered horizontally and vertically in the button.

      Currently this is a string value. We are waiting for
      support for enums:
      https://bugreports.qt-project.org/browse/QTBUG-14861
    */
    property string iconPosition: "left"

    /*!
      Specifies whether the combo list is expanded or not. The default falue is
      false.
      */
    property bool expanded: false

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

        The default value is \l collapsedHeight + 19.5 GU, so at least 3 ListItems
        can fit in the combo list.
        \sa collapsedHeight
      */
    property real expandedHeight: collapsedHeight + units.gu(19.5)

    /*!
      The property holds the maximum combo list height allowed based on the
      \l expandedHeight and \l collapsedHeight values. It is a convenience property
      that can be used to size the combo list content.
      \qml
      import QtQuick 2.4
      import Ubuntu.Components 1.3
      import Ubuntu.Components.ListItems 1.3
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
        Property holding the list of items to be shown in the combo list. Being
        a default property children items declared will land in the combo list.
        \note The component is not responsible for layouting the content. It only
        provides scrolling abilities for the case the content exceeds the defined
        expanded height.
      */
    default property alias comboList: comboListHolder.data

    styleName: "ComboButtonStyle"

    Component.onCompleted: {
        // update mouse area to report clicks only on the main button area
        // area excluding dropDown button and combo list
        // we must do separate bindings as __mouseArea is a read-only property
        __mouseArea.anchors.fill = undefined;
        __mouseArea.anchors.left = Qt.binding(function() {return combo.left;});
        __mouseArea.anchors.top = Qt.binding(function() {return combo.top;});
        __mouseArea.anchors.right = Qt.binding(function() {return combo.right;});
        __mouseArea.anchors.rightMargin = Qt.binding(function() {return combo.__styleInstance.dropDownWidth;});
        __mouseArea.height = Qt.binding(function() {return collapsedHeight;});
        // for autopilot, set the main button name
        __mouseArea.objectName = "combobutton_mainbutton";

        // bind a height calculation to avoid unwanted change
        combo.height = Qt.binding(function() {return collapsedHeight + __styleInstance.comboListPanel.height});
    }

    // dropdown button
    AbstractButton {
        id: dropDown
        objectName: "combobutton_dropdown"
        anchors {
            right: parent.right
            top: parent.top
        }
        width: combo.__styleInstance ? combo.__styleInstance.dropDownWidth : 0
        height: combo.collapsedHeight
        // open dropdown when pressed, not when clicked
        onClicked: {
            // toggle expanded
            combo.expanded = !combo.expanded;
        }
    }

    // expansion list
    Flickable {
        id: comboHolder
        objectName: "combobutton_contentflicker"
        parent: combo.__styleInstance ? combo.__styleInstance.comboListHolder : combo
        anchors.fill: parent
        interactive: combo.expanded && !contentIsFlickable() && (combo.expandedHeight > 0)
        flickableDirection: Flickable.VerticalFlick
        contentHeight: comboListHolder.height

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

            // stretch children width to holder's width
            // must do binding to height manually to avoid binding loops caused
            // by the vertical stretching when the component is a single flickable
            property bool stretch: (combo.expandedHeight > 0 && comboHolder.contentIsFlickable())
            onStretchChanged: stretchChildren()
            onChildrenChanged: stretchChildren()
            function stretchChildren() {
                for (var i in comboListHolder.children) {
                    var child = comboListHolder.children[i];
                    if (comboListHolder.stretch) {
                        child.anchors.left = undefined;
                        child.anchors.right = undefined;
                        child.anchors.fill = comboListHolder;
                        comboListHolder.height = Qt.binding(function() {
                            return combo.comboListHeight;
                        });
                    } else {
                        child.anchors.left = comboListHolder.left;
                        child.anchors.right = comboListHolder.right;
                        comboListHolder.height = Qt.binding(function() {
                            return comboListHolder.childrenRect.height;
                        });
                    }
                }
            }
        }
    }
    Scrollbar {
        flickableItem: comboHolder
    }

    /*
      Drive the expansion height of the combo list holder
      This is a common functionaity in all combo buttons, therefore we can have it
      in the main component.
      */
    Binding {
        target: combo.__styleInstance.comboListPanel
        property: "height"
        value: {
            if (!expanded) {
                return 0;
            }
            if (comboListHolder.stretch) {
                return combo.comboListHeight + combo.__styleInstance.comboListMargin;
            }

            var h = comboListHolder.height;
            var max = combo.comboListHeight + combo.__styleInstance.comboListMargin;
            if (combo.expandedHeight < 0) {
                return max;
            }

            return MathUtils.clamp(h, 0, max);
        }
    }
}
