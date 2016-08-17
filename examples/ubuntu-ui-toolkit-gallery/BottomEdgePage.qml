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

Template {
    id: page
    header: PageHeader {
        title: i18n.tr("Bottom Edge")
        trailingActionBar.actions: [
            bottomEdge.hint.action
        ]
    }

    TemplateSection {
        title: "BottomEdgeHint"
        className: "BottomEdgeHint"

        TemplateRow {
            title: i18n.tr("On clicked")
            Row {
                spacing: units.gu(1)
                CheckBox {
                    id: contentToLayout
                    text: i18n.tr("push content into the column")
                    enabled: bottomEdge.hint.status >= BottomEdgeHint.Active
                }
                Label {
                    text: contentToLayout.text
                    anchors.verticalCenter: contentToLayout.verticalCenter
                }
            }
        }
    }

    TemplateSection {
        title: "BottomEdge"
        className: "BottomEdge"

        TemplateRow {
            title: i18n.tr("Content")
            Row {
                spacing: units.gu(1)
                CheckBox {
                    id: preloadContent
                    text: i18n.tr("preload content")
                    checked: bottomEdge.preloadContent
                    onTriggered: bottomEdge.preloadContent = checked
                }
                Label {
                    text: preloadContent.text
                    anchors.verticalCenter: preloadContent.verticalCenter
                }
            }
        }

        TemplateFlow {
            title: i18n.tr("Top")
            Slider {
                id: bottomEdgeHeight
                width: Math.min(units.gu(30), parent.width)
                maximumValue: page.height
                value: bottomEdge.height
                onValueChanged: bottomEdge.height = value
            }
        }

        TemplateFlow {
            title: i18n.tr("Regions")
            Slider {
                id: regionCount
                width: Math.min(units.gu(20), parent.width)
                maximumValue: 3.0
                live: true
            }
        }
        Repeater {
            id: regionConfig
            model: regionCount.value.toFixed(0)
            TemplateRow {
                title: i18n.tr("Region #%1").arg(index)
                property int regionIndex: index
                Repeater {
                    model: ["from", "to"]
                    Row {
                        spacing: units.gu(2)
                        Label {
                            text: i18n.tr(modelData)
                        }
                        TextField {
                            id: regionFrom
                            text: bottomEdge.regions[regionIndex][modelData]
                            validator: DoubleValidator {bottom: 0.0; top: 1.0; decimals: 2; locale: "d.d"}
                            inputMethodHints: Qt.ImhPreferNumbers | Qt.ImhFormattedNumbersOnly
                            width: units.gu(7)
                            hasClearButton: false
                            errorHighlight: true
                            onAccepted: bottomEdge.regions[regionIndex][modelData] = text
                            onTextChanged: {
                                if (regionFrom.acceptableInput) {
                                    bottomEdge.regions[regionIndex][modelData] = parseFloat(text);
                                }
                            }
                            onActiveFocusChanged: if (activeFocus) selectAll()
                        }
                    }
                }
            }
        }
    }

    BottomEdge {
        id: bottomEdge
        // make sure it doesn't land inside the flickable
        parent: page
        // hint
        hint {
            action: Action {
                text: "Demo content"
                iconName: "stock_message"
                onTriggered: bottomEdge.commit()
            }
            flickable: page.flickable
        }
        contentComponent: bottomEdgeContent

        onCommitCompleted: {
            if (contentToLayout.checked && contentToLayout.enabled) {
                page.pageStack.addPageToCurrentColumn(page, contentComponent);
                collapse();
            }
        }

        regions: [
            BottomEdgeRegion {
                objectName: "CustomRegion1"
                enabled: regionConfig.model >= 1
                to: 0.3
                property color baseColor: UbuntuColors.silk
                contentComponent: Rectangle {
                    PageHeader {
                        title: i18n.tr("CustomRegion #1")
                    }
                    width: bottomEdge.width - units.gu(10)
                    height: bottomEdge.height
                    color: UbuntuColors.blue
                }
            },
            BottomEdgeRegion {
                objectName: "CustomRegion2"
                enabled: regionConfig.model >= 2
                from: 0.3
                to: 0.6
                contentComponent: Rectangle {
                    PageHeader {
                        title: i18n.tr("CustomRegion #2")
                    }
                    width: bottomEdge.width - units.gu(30)
                    height: bottomEdge.height
                    color: UbuntuColors.red
                }
            },
            BottomEdgeRegion {
                objectName: "CustomRegion3"
                enabled: regionConfig.model >= 3
                from: 0.6
            },
            // default region, mimics the default setup
            BottomEdgeRegion {
                objectName: "DefaultRegion"
                enabled: regionConfig.model <= 0
                from: 0.3
            }
        ]

        Component {
            id: bottomEdgeContent
            Page {
                height: bottomEdge.height
                header: PageHeader {
                    title: {
                        var state = "UNDEFINED";
                        switch (bottomEdge.status) {
                        case BottomEdge.Hidden: state = "Hidden"; break;
                        case BottomEdge.Revealed: state = "Revealed"; break;
                        case BottomEdge.Committed: state = "Committed"; break;
                        }
                        return bottomEdge.activeRegion
                          ? i18n.tr("Within region '%1', status: %2").arg(bottomEdge.activeRegion.objectName).arg(state)
                          : i18n.tr("Not in any active region, status: %1").arg(state);
                    }
                }
                Rectangle {
                    anchors.fill: parent
                    anchors.margins: units.gu(1)
                    color: bottomEdge.activeRegion && bottomEdge.activeRegion.hasOwnProperty("baseColor") ?
                               bottomEdge.activeRegion.baseColor : Qt.rgba(0.5, 1, bottomEdge.dragProgress, 1)
                }
            }
        }
    }
}
