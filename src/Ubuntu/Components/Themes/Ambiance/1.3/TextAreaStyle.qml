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

// frame
// FIXME: stabilize API
Item {
    id: visuals
    // style properties
    // FIXME: needs type checking in themes to define the proper type to be used
    // if color type is used, alpha value gets lost

    property color color: styledItem.enabled
                          ? ((styledItem.activeFocus || styledItem.highlighted)
                            ? theme.palette.selected.fieldText
                            : theme.palette.normal.fieldText)
                          : theme.palette.disabled.fieldText
    property color selectedTextColor: styledItem.enabled ? theme.palette.selected.selectionText : theme.palette.disabled.selectionText
    property color selectionColor: styledItem.enabled ? theme.palette.selected.selection : theme.palette.disabled.selection
    /*!
      Background fill color
      */
    property color borderColor: theme.palette.normal.base
    property color backgroundColor: styledItem.enabled
                                        ? ((styledItem.activeFocus || styledItem.highlighted)
                                            ? theme.palette.selected.field
                                            : theme.palette.normal.field)
                                        : theme.palette.disabled.field
    property color errorColor: styledItem.enabled ? theme.palette.normal.negative : theme.palette.disabled.negative

    /*!
      Spacing between the frame and the text editor area
      */
    property real frameSpacing: units.gu(1)
    // Obsolete
    property alias overlaySpacing: visuals.frameSpacing

    // style body
    anchors.fill: parent
    objectName: "textarea_style"

    z: -1

    /*!
      Text input background
      */
    FocusShape {
    }

    property Component background: UbuntuShape {
        property bool error: (styledItem.hasOwnProperty("errorHighlight") && styledItem.errorHighlight && !styledItem.acceptableInput)
        onErrorChanged: (error) ? visuals.errorColor : visuals.backgroundColor;
        backgroundColor: visuals.borderColor
        aspect: UbuntuShape.Flat
        visible: !styledItem.readOnly
        anchors.fill: parent

        UbuntuShape {
            anchors.fill: parent
            anchors.margins: parent.visible ? units.dp(0.5) : 0
            radius: units.gu(1.7)
            backgroundColor: visuals.backgroundColor
            aspect: UbuntuShape.Flat
            source: ShaderEffectSource {
                sourceItem: styledItem.__rightScrollbar ? styledItem.__rightScrollbar : null
                hideSource: true
            }
            sourceFillMode: UbuntuShape.Pad
            sourceHorizontalAlignment: UbuntuShape.AlignRight
        }
    }

    Loader {
        id: backgroundLoader
        sourceComponent: background
        anchors.fill: parent
    }
}
