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

import QtQuick 2.4
import Ubuntu.Components 1.3
import QtQml.Models 2.1

Template {
    id: colorsTemplate
    objectName: "colorsTemplate"

    TemplateSection {
        id: paletteSection

        title: "Palette"
        className: "Palette"
        documentation: "qml-palette.html"

        property var palettes: ["normal", "disabled", "selected", "selectedDisabled", "highlighted", "focused"]
        property var paletteValues: [["background", ["backgroundText", "backgroundSecondaryText", "backgroundTertiaryText"]],
            ["base", ["baseText"]],
            ["foreground", ["foregroundText"]],
            ["field", ["fieldText"]],
            ["focus", ["focusText"]],
            ["activity", ["activityText"]],
            ["negative", ["negativeText"]],
            ["overlay", ["overlayText", "overlaySecondaryText"]],
            ["positive", ["positiveText"]],
            ["raised", ["raisedText", "raisedSecondaryText"]],
            ["selection", ["selectionText"]],
            ["position", ["positionText"]],
        ]

        Flow {
            id: paletteFlow
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: units.gu(2)
            property var previewed

            Repeater {
                model: paletteSection.palettes
                Column {
                    spacing: units.gu(1)
                    width: paletteColorGrid.width
                    property var palette: theme.palette[modelData]

                    Label {
                        text: "theme.palette." + modelData
                        textSize: Label.Small
                    }

                    Grid {
                        id: paletteColorGrid
                        spacing: units.gu(0)

                        Repeater {
                            model: paletteSection.paletteValues

                            Item {
                                id: paletteColor
                                property string mainColor: modelData[0]
                                property var textColors: modelData[1]
                                property bool previewed: paletteFlow.previewed == paletteColor
                                width: units.gu(8)
                                height: units.gu(5)
                                z: previewed ? 10 : 0

                                MouseArea {
                                    id: colorHoverArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: paletteFlow.previewed = paletteColor
                                    onContainsMouseChanged: {
                                        if (containsMouse) {
                                            paletteFlow.previewed = paletteColor;
                                        } else {
                                            if (paletteFlow.previewed == paletteColor) {
                                                paletteFlow.previewed = null;
                                            }
                                        }
                                    }
                                }

                                Rectangle {
                                    width: paletteColor.width + (previewed ? units.gu(26) : 0)
                                    height: paletteColor.height + (previewed ? units.gu(2.5) : 0)
                                    Behavior on height { UbuntuNumberAnimation {duration: UbuntuAnimation.SnapDuration} }
                                    color: theme.palette.normal.background
                                    parent: previewed ? colorsTemplate : paletteColor
                                    x: Math.max(0, paletteColor.mapToItem(parent, 0, 0).x + (previewed ? (paletteColor.width-width)/2 : 0))
                                    y: paletteColor.mapToItem(parent, 0, 0).y + (previewed ? (paletteColor.height-height)/2 : 0)

                                    Rectangle {
                                        anchors.fill: parent
                                        color: palette ? palette[mainColor] : "transparent"
                                        border.width: color == theme.palette.normal.background ? units.dp(1) : 0
                                        border.color: theme.palette.normal.backgroundText
                                    }

                                    Column {
                                        anchors {
                                            left: parent.left
                                            leftMargin: units.gu(0.5)
                                            right: parent.right
                                            top: parent.top
                                            topMargin: units.gu(0.5)
                                        }

                                        Repeater {
                                            model: textColors
                                            Label {
                                                width: previewed ? implicitWidth : parent.width
                                                text: previewed ? modelData : modelData.slice(0, 10)
                                                color: palette ? palette[modelData] : "transparent"
                                                textSize: previewed ? Label.Medium : Label.XSmall
                                                elide: Text.ElideRight
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    TemplateSection {
        title: "UbuntuColors"
        className: "UbuntuColors"

        // Except orange, none of the colors from UbuntuComponents 1.0
        // are included because they may be deprecated soon.
        TemplateRow {
            title: i18n.tr("Orange")
            UbuntuShape {
                backgroundColor: UbuntuColors.orange
            }
        }
        TemplateRow {
            title: i18n.tr("Porcelain")
            UbuntuShape {
                backgroundColor: UbuntuColors.porcelain
            }
        }
        TemplateRow {
            title: i18n.tr("Silk")
            UbuntuShape {
                backgroundColor: UbuntuColors.silk
            }
        }
        TemplateRow {
            title: i18n.tr("Ash")
            UbuntuShape {
                backgroundColor: UbuntuColors.ash
            }
        }
        TemplateRow {
            title: i18n.tr("Graphite")
            UbuntuShape {
                backgroundColor: UbuntuColors.graphite
            }
        }
        TemplateRow {
            title: i18n.tr("Slate")
            UbuntuShape {
                backgroundColor: UbuntuColors.slate
            }
        }
        TemplateRow {
            title: i18n.tr("Inkstone")
            UbuntuShape {
                backgroundColor: UbuntuColors.inkstone
            }
        }
        TemplateRow {
            title: i18n.tr("Jet")
            UbuntuShape {
                backgroundColor: UbuntuColors.jet
            }
        }
        TemplateRow {
            title: i18n.tr("Red")
            UbuntuShape {
                backgroundColor: UbuntuColors.red
            }
        }
        TemplateRow {
            title: i18n.tr("Green")
            UbuntuShape {
                backgroundColor: UbuntuColors.green
            }
        }
        TemplateRow {
            title: i18n.tr("Blue")
            UbuntuShape {
                backgroundColor: UbuntuColors.blue
            }
        }
        TemplateRow {
            title: i18n.tr("Purple")
            UbuntuShape {
                backgroundColor: UbuntuColors.purple
            }
        }
    }
}
