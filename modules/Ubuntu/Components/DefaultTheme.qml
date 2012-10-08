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

Theme{

    // the item's children are stored in inverse order, so the last child under Item is
    // the first one evaluated. Therefore all overloading style should be defined prior
    // to the style to be overload

    Rule {
        // this style component defines the custom properties applied on the component
        // only the altered property is defined, the rest should be taken from the
        // .Button style
        selector: ".button"
        style: ButtonStyle {
            color: "#a3e5e8";
            shapeDark: "artwork/ButtonShapeDark.png";
            shapeNormal: "artwork/ButtonShape.png";
            borderIdle: "artwork/ButtonBorderIdle.png";
            borderPressed: "artwork/ButtonBorderPressed.png";
            borderDarkIdle: "artwork/ButtonBorderDarkIdle.png";
            borderDarkPressed: "artwork/ButtonBorderDarkPressed.png";
        }
    }

    Rule {
        // this style component defines the default visuals of a Button control
        selector: ".Button"
        style: ButtonStyle {
            color: "#e3e5e8";
            shapeDark: "artwork/ButtonShapeDark.png";
            shapeNormal: "artwork/ButtonShape.png";
            borderIdle: "artwork/ButtonBorderIdle.png";
            borderPressed: "artwork/ButtonBorderPressed.png";
            borderDarkIdle: "artwork/ButtonBorderDarkIdle.png";
            borderDarkPressed: "artwork/ButtonBorderDarkPressed.png";
        }

        delegate: Component {ButtonDelegate{}}
    }
}
