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

      Note that these values do not affect the size of the highlighted item.
      That one is specified by the style component of the Picker.
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
    readonly property alias itemList: loader.item

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

        // update curentItem automatically when selectedIndex changes
        Binding {
            target: loader.item
            property: "currentIndex"
            value: picker.selectedIndex
            when: loader.item && loader.status === Loader.Ready
        }
//        Connections {
//            target: picker
//            onSelectedIndexChanged: loader.item.currentIndex = picker.selectedIndex
//        }

        // live selectedIndex updater
//        Binding {
//            target: picker
//            property: "selectedIndex"
//            value: loader.item.currentIndex
//            when: loader.completed && (picker.model !== undefined) &&
//                  (picker.live || (picker.__clickedIndex === loader.item.currentIndex))
//        }
        // non-live selectedIndex updater
        Connections {
            target: loader.item
            ignoreUnknownSignals: true
            onMovementEnded: {
                print("move stopped")
                if (!picker.live) {
                    picker.selectedIndex = loader.item.currentIndex;
                }
            }
            onCurrentIndexChanged: {
                if (picker.live) {
                    print("ci2si " + picker.objectName + "/" + loader.item.currentIndex)
                    picker.selectedIndex = loader.item.currentIndex;
                } else if (!picker.model.count) {
                    // model reset?
                    print('reset ' + picker.objectName + ": " + loader.item.currentIndex)
                    picker.selectedIndex = loader.item.currentIndex;
                } else if (picker.__clickedIndex > 0 && picker.__clickedIndex === loader.item.currentIndex){
                    print('clicked ' + picker.objectName)
                    picker.selectedIndex = loader.item.currentIndex;
                } else if (modelWatcher.removing) {
                    picker.selectedIndex = loader.item.currentIndex;
                    modelWatcher.removing = false;
                } else {
                    print("unhandled change on "+picker.objectName+": " + loader.item.currentIndex)
                }

//                if (picker.live || (picker.__clickedIndex === loader.item.currentIndex)) {
//                    picker.selectedIndex = loader.item.currentIndex;
//                    picker.__clickedIndex = -1;
//                }
            }
            onModelChanged: {
                modelWatcher.connectModel(picker.model);
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
            modelWatcher.connectModel(picker.model);
        }
    }

    // circular list
    Component {
        id: wrapAround
        PathView {
            id: pView
            objectName: "Picker_WrapAround"
            // property declared for PickerDelegate to be able to access the main component
            property Item pickerItem: picker
            anchors {
                top: parent ? parent.top : undefined
                bottom: parent ? parent.bottom : undefined
                horizontalCenter: parent ? parent.horizontalCenter : undefined
            }
            width: parent ? MathUtils.clamp(picker.itemWidth, 0, parent.width) : 0
            clip: true

            model: picker.model
            delegate: picker.delegate
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
            objectName: "Picker_Linear"
            // property declared for PickerDelegate to be able to access the main component
            property Item pickerItem: picker
            anchors {
                top: parent ? parent.top : undefined
                bottom: parent ? parent.bottom : undefined
                horizontalCenter: parent ? parent.horizontalCenter : undefined
            }
            width: parent ? MathUtils.clamp(picker.itemWidth, 0, parent.width) : 0
            clip: true

            model: picker.model
            delegate: picker.delegate
            currentIndex: picker.selectedIndex

            preferredHighlightBegin: (height - picker.itemHeight) / 2
            preferredHighlightEnd: preferredHighlightBegin + picker.itemHeight
            highlightRangeMode: ListView.StrictlyEnforceRange
            highlightMoveDuration: 300
            flickDeceleration: 100
        }
    }

    /*
     Watch Picker's model for elements removal, as that is not handled properly
     by the ListView. If currentIndex fails into the removed model area, PathView will
     position its currentIndex to the last item, but ListView will leave it somewhere
     random. In any case, currentItemChange is not triggered. Also, ListView does
     not report currentIndex changes when the
     */
    QtObject {
        id: modelWatcher

        property var prevModel
        property bool removing: false

        function connectModel(model) {
            if (prevModel && Object.prototype.toString.call(prevModel) === "[object Object]") {
                disconnectModel(prevModel);
            }
            prevModel = model;
            // check if the model is derived from QAbstractListModel
            if (Object.prototype.toString.call(model) === "[object Object]") {
                model.rowsAboutToBeRemoved.connect(itemsAboutToRemove);
                model.rowsInserted.connect(itemsAdded);
            }
        }

        function disconnectModel(model) {
            model.rowsAboutToBeRemoved.disconnect(itemsAboutToRemove);
            model.rowsInserted.disconnect(itemsAdded);
        }

        function itemsAdded(parent, start, end) {
            if (loader.item.currentIndex < 0) {
//                loader.item.currentIndex = 0;
            }
        }

        function itemsAboutToRemove(parent, start, end) {
            print("model count="+picker.itemList.count + ", start="+start+", end="+end)
            if ((end - start + 1) === picker.itemList.count) {
                // cleanup, move selection to 0
                print("ABOUT TO CLEAR")
//                picker.itemList.currentIndex = 0;
                removing = true;
            } else if (picker.selectedIndex >= start) {
                print("ABOUT TO REMOVE")
                removing = true;
//                picker.itemList.currentIndex = start - 1;
            }
        }
    }

}
