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
import Ubuntu.Components 0.1

Template {
    objectName: "Ubuntu Shape"
    title: i18n.tr("Ubuntu Shape")

    Column {
        spacing: units.gu(4)


        TemplateRow {
            title: i18n.tr("Radius")
            height: units.gu(8)

            UbuntuShape {
                color: "#c94212"
                radius: "small"

                Label {
                    anchors.centerIn: parent
                    text: "small"
                    fontSize: "x-small"
                    color: "#f1f1f1"
                }
            }

            UbuntuShape {
                color: "#c94212"
                radius: "medium"

                Label {
                    anchors.centerIn: parent
                    text: "medium"
                    fontSize: "x-small"
                    color: "#f1f1f1"
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Color")
            height: units.gu(8)

            UbuntuShape {
                color: "#5da357"
            }

            UbuntuShape {
                color: "lightblue"
            }

            UbuntuShape {
                color: "purple"
            }

            UbuntuShape {
                color: "darkgrey"
            }
        }

        TemplateRow {
            title: i18n.tr("Image")
            height: units.gu(8)

            UbuntuShape {
                width: units.gu(12)
                image: Image {
                    fillMode: Image.Stretch
                    source: "map_icon.png"
                }
            }

            UbuntuShape {
                width: units.gu(12)
                image: Image {
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                    horizontalAlignment: Image.AlignLeft
                    verticalAlignment: Image.AlignTop
                }
            }

            UbuntuShape {
                width: units.gu(12)
                image: Image {
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                    horizontalAlignment: Image.AlignRight
                    verticalAlignment: Image.AlignBottom
                }
            }

            UbuntuShape {
                width: units.gu(12)
                image: Image {
                    objectName: "MAP"
                    source: "map_icon.png"
                    fillMode: Image.PreserveAspectCrop
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Resizing")
            height: units.gu(10)

            UbuntuShape {
                color: "#c94212"
            }

            UbuntuShape {
                color: "#c94212"
                width: units.gu(10)
                height: units.gu(4)
            }

            UbuntuShape {
                color: "#c94212"
                width: units.gu(20)
                height: units.gu(10)
            }
        }

        TemplateRow {
            title: i18n.tr("Children")
            height: units.gu(8)


            EvilUbuntuShape {
                id: fboShape
//                clip: true
                radius: "medium"
                width: units.gu(10)
                height: units.gu(8)
//                item: rect

                item: Rectangle {
                    id: rect
                    color: "purple"
//                    parent: fboShape
                    anchors.fill: parent
                    Column {
                        anchors.centerIn: parent
                        spacing: units.gu(1)
                        Button {
                            text: "Button one"
                            onClicked: print("bla")
                            width: units.gu(14)
                        }
                        Button {
                            text: "Button two"
                            onClicked: print("bla")
                            width: units.gu(14)
                        }
                    }
                }

//                ShaderEffectSource {
//                    smooth: false // prevent linear interpolation
//                    id: fbo
//                    objectName: "FBO"
//                    hideSource: true
//                    sourceItem: rect
//                    format: ShaderEffectSource.RGBA
//                    live: true
//                    width: parent.width
//                    height: parent.height
//                    x: width
//                }
            }
        }
    }
}
