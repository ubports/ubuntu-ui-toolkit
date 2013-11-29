/*
 * Copyright 2013 Canonical Ltd.
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
import "../" 0.1

/*!
    \qmltype Picker
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief Picker is a slot-machine style value selection component.

    The Picker lists the elements specified by the \l model using the \l delegate
    vertically using a slot-machine tumbler-like list. The selected item is
    always the one in the center of the component, and it is represented by the
    \l selectedIndex property.

    The elements can be either in a circular list or in a normal list.

    Delegates must be composed using PickerDelegate.

    Example:
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Picker {
        model: ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"]
        delegate: PickerDelegate {
            Label {
                text: modelData
            }
        }
        selectedIndex: 5
        onSelectedIndexChanged: {
            print("selected month: " + selectedIndex);
        }
    }
    \endqml

    \b Note: the \l selectedIndex must be set explicitly to the desired index if
    the model is set, filled or changed after the component is complete. In the
    following example the selected item must be set after the model is set.

    \qml
    Picker {
        selectedIndex: 5 // this will be set to 0 at the model completion
        delegate: PickerDelegate {
            Label {
                text: modelData
            }
        }
        Component.onCompleted: {
            var stack = [];
            for (var i = 0; i < 10; i++) {
                stack.push("Line " + i);
            }
            model = stack;
            // selectedIndex must be set explicitly
            selectedIndex = 3;
        }
    }
    \endqml
 */

StyledItem {
    id: picker

    /*!
      Property specifying whether the tumbler list is wrap-around (\a true), or
      normal (\a false). Default value is true.
      */
    property bool circular: true

    /*!
      Specifies the model listing the content of the picker.
      */
    property var model

    /*!
      The delegate visualizing the model elements. Any kind of component can be
      used as delegate, however it is recommended to use \l PickerDelegate, which
      integrates selection functionality into the Picker.
      */
    property Component delegate

    /*!
      Specifies the item to be overlaid to the picker's highlight defined by
      the style. The highlight is considered to be placed vertically in the middle
      of the Picker's tumbler, and the component specified in this property will
      be overlaid above it.
      */
    property Item highlightOverlay

    /*!
      The property holds the index of the selected item
      */
    property int selectedIndex

    /*!
      Defines whether the \l selectedIndex should be updated while the tumbler
      changes the selected item during draggingm or only when the tumbler's motion
      ends. The default behavior is non-live update.
      */
    property bool live: false

    /*!
      \qmlproperty real itemHeight
      \qmlproperty real itemWidth
      These properties define the picker list item's width and height. This is
      used in calculating the amount of components to be shown in the tumbler,
      and it is also set as default width and height for the PickerDelegate.
      The default values are covering the entire width of the Picker and 4.5 GU
      as height.
      */

    /*! \internal */
    property real itemWidth: loader.width
    /*! \internal */
    property real itemHeight: units.gu(4.5)

    /*!
      \qmlproperty Item itemList
      The property holds the component listing the model content using the given
      delegate. It can either be a ListView or a PathView depending whether the
      picker is chosen to be circular or linear.
      */
    property alias itemList: loader.item

    implicitWidth: units.gu(8)
    implicitHeight: units.gu(20)

    style: Theme.createStyleComponent("PickerStyle.qml", picker)

    /*! \internal */
    property int __clickedIndex: -1

    // tumbler
    Loader {
        id: loader
        asynchronous: false
        parent: __styleInstance.hasOwnProperty("tumblerHolder") ? __styleInstance.tumblerHolder : picker
        anchors.fill: parent
        sourceComponent: circular ? wrapAround : linear

        property bool completed: item && (status === Loader.Ready)

        // live selectedIndex updater
        Binding {
            target: picker
            property: "selectedIndex"
            value: loader.item.currentIndex
            when: loader.completed && (picker.model !== undefined) && picker.live
        }
        // non-live selectedIndex updater
        Connections {
            target: loader.item
            ignoreUnknownSignals: true
            onMovementEnded: {
                if (!picker.live) {
                    picker.selectedIndex = loader.item.currentIndex;
                }
            }
            onCurrentIndexChanged: {
                if (!picker.live && picker.__clickedIndex === loader.item.currentIndex) {
                    picker.selectedIndex = loader.item.currentIndex;
                    picker.__clickedIndex = -1;
                }
            }
            onModelChanged: {
                loader.moveToIndex((loader.completed) ? 0 : picker.selectedIndex);
                if (loader.completed && !picker.live) {
                    picker.selectedIndex = 0;
                }
            }
        }

        function modelSize() {
            return loader.item.model.hasOwnProperty("count") ? loader.item.model.count : loader.item.model.length;
        }

        function moveToIndex(toIndex) {
            var count = (loader.item && loader.item.model) ? modelSize() : -1;
            if (completed && count > 0) {
                if (QuickUtils.className(loader.item) === "QQuickListView") {
                    loader.item.currentIndex = toIndex;
                    return;
                } else {
                    loader.item.positionViewAtIndex(count - 1, PathView.Center);
                    loader.item.positionViewAtIndex(toIndex, PathView.Center);
                }
            }
        }

        Component.onCompleted: {
            loader.completed = true;
            if (item) {
                loader.item.currentIndex = picker.selectedIndex;
                moveToIndex(picker.selectedIndex);
            }
        }
    }

    // highlight component used for calculations
    Component {
        id: highlightComponent
        Item {
            width: picker.itemWidth
            height: picker.itemHeight
        }
    }

    // circular list
    Component {
        id: wrapAround
        PathView {
            id: pView
            // property declared for PickerDelegate
            property Item pickerItem: picker
            anchors {
                top: parent ? parent.top : undefined
                bottom: parent ? parent.bottom : undefined
                horizontalCenter: parent ? parent.horizontalCenter : undefined
            }
            width: picker.itemWidth
            clip: true

            model: picker.model
            delegate: picker.delegate
            highlight: highlightComponent
            currentIndex: picker.selectedIndex
            // put the currentItem to the center of the view
            preferredHighlightBegin: 0.5
            preferredHighlightEnd: 0.5

            pathItemCount: pView.height / picker.itemHeight + 1
            snapMode: PathView.SnapToItem
            flickDeceleration: 100

            property int contentHeight: pathItemCount * picker.itemHeight
            path: Path {
                startX: pView.width / 2
                startY: -(pView.contentHeight - pView.height) / 2
                PathLine {
                    x: pView.width / 2
                    y: pView.height + (pView.contentHeight - pView.height) / 2
                }
            }
        }
    }

    // linear list
    Component {
        id: linear
        ListView {
            id: lView
            // property declared for PickerDelegate
            property Item pickerItem: picker
            anchors {
                top: parent ? parent.top : undefined
                bottom: parent ? parent.bottom : undefined
                horizontalCenter: parent ? parent.horizontalCenter : undefined
            }
            width: picker.itemWidth
            clip: true

            model: picker.model
            delegate: picker.delegate
            highlight: highlightComponent
            currentIndex: picker.selectedIndex

            preferredHighlightBegin: (height - picker.itemHeight) / 2
            preferredHighlightEnd: preferredHighlightBegin + picker.itemHeight
            highlightRangeMode: ListView.StrictlyEnforceRange
            highlightMoveDuration: 300
            flickDeceleration: 100
        }
    }
}
