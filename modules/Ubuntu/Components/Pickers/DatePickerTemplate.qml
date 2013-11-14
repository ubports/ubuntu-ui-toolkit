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
import "dateUtils.js" as DateUtils
import "../" 0.1

/*
  Picker template for DatePicker
  It is expected from the model to have the following API:
  ListModel {
      property bool circular: to specify whether the model data is made for circular Picker
      [property bool autoExtend: to specify whether the model can extend itself (non-circular model only)]
      function reset(date, minimum, maximum) - reset model with
      function indexOf(date) - returns the model index of the date
      function dateFromModel(date, index) - creates a Date object from the model index, relative to the date
      function text(date, value, format) - returns the string of the model index relative to the date,
                    using the specified format (long, short, narrow)
  }
  */
Picker {
    id: item
    property Item picker
    property bool completed: false
    property var updatePickerWhenChanged: null
    property string format: limits ? limits.format(width) : "long"
    property var limits: null

    style: Theme.createStyleComponent("FlatPickerStyle.qml", item)

    enabled: model.count > 1
    live: picker.live
    circular: model.circular

    delegate: PickerDelegate {
        style: Item{}
        Label {
            text: item.model.text(picker.date, modelData, format);
//            color: highlighted ? "red" : Theme.palette.normal.backgroundText
//            scale: highlighted ? 1.1 : 1
            smooth: true
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        Component.onCompleted: {
            if (model && model.hasOwnProperty("autoExtend") && model.autoExtend && (index === (model.count - 1))) {
                model.extend(modelData + 1);
            }
        }
    }

    Binding {
        target: parent
        property: "style"
        value: picker.pickerStyle
        when: (picker.pickerStyle !== null)
    }

    onSelectedIndexChanged: {
        if (!completed || !visible) return;
        picker.date = model.dateFromModel(picker.date, selectedIndex);
        if (updatePickerWhenChanged) {
            updatePickerWhenChanged.update(picker.date);
        }
    }

    function resetModel() {
        if (!visible) return;

        model.reset(picker.date, picker.minimum, picker.maximum);
        selectedIndex = model.indexOf(picker.date);
    }
}
