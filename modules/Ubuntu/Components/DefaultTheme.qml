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

import QtQuick 1.1
import Ubuntu.Components 0.1

Theme{

    // the item's children are stored in inverse order, so the last child under Item is
    // the first one evaluated. Therefore all overloading style should be defined prior
    // to the style to be overload

    Rule {
        // this style component defines the custom properties applied on the component
        // only the altered property is defined, the rest should be taken from the
        // .Button style
        selector: ".button"
        style: QtObject {
            property variant color: control.pressed ? "green" : control.hovered ? "tan" : "#e3e5e8"//"#e3e5e8"
            property variant borderShape: control.darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
            property variant borderImage: (control.darkBorder) ? (control.pressed ? "artwork/ButtonBorderDarkPressed.png" : "artwork/ButtonBorderDarkIdle.png")
                    : (control.pressed ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png")
        }
    }

    Rule {
        // this style component defines the default visuals of a Button control
        selector: ".Button"
        style: QtObject {
            property variant color: control.pressed ? "purple" : control.hovered ? "cyan" : "pink"//"#e3e5e8"
            property variant borderShape: control.darkBorder ? "artwork/ButtonShapeDark.png" : "artwork/ButtonShape.png"
            property variant borderImage: (control.darkBorder) ? (control.pressed ? "artwork/ButtonBorderDarkPressed.png" : "artwork/ButtonBorderDarkIdle.png")
                    : (control.pressed ? "artwork/ButtonBorderPressed.png" : "artwork/ButtonBorderIdle.png")
        }

        delegate: Component {ButtonStyle{}}
    }


    // test rulez
    Rule {
        selector: ".tframe"
        style: QtObject {
            property color color: "tan"
            property color border: "blue"
        }
    }

    Rule {
        selector: ".tframe > .tbutton"
        delegate: Rectangle {
            z: -1
            anchors.fill: parent
            radius: 8
            border.color: "pink"
            border.width: 4
            color: control.pressed ? "grey" : "yellow"
            smooth: true

            MouseArea {
                anchors.fill: parent
                onClicked: control.clicked()
            }
        }
    }
    Rule {
        selector: ".tbox .tframe .tbutton"
        style: Item{}
    }

    Rule {
        selector: ".tframe > .tbutton#blah"
        style: Item{}
    }

    Rule {
        selector: ".ttoolbar > .tbutton"
        style: Item {}
    }

}
