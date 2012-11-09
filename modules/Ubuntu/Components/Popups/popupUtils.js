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

.import "../mathUtils.js" as MathUtils

function open(component, caller) {
    // TODO: Check that component is a valid component, and the object
    //  can be created.
    var popup = component.createObject(QuickUtils.rootObject, { "caller": caller });
    popup.show();

    popup.onVisibleChanged.connect(popup.__closeIfHidden);
    return popup;
}

//function close(popupObject) {
//    popupObject.hide();
//    popupObject.__closeIfHidden();
//}


// TODO: close function.
// TODO: support for no caller?
function Positioning(item, area, caller, edgeMargins, callerMargins) {

    // all coordinate computation are relative inside "area".

    this.horizontalCenter = function() {
        return area.width/2 - item.width/2;
    }

    this.verticalCenter = function() {
        return area.height/2 - item.height/2;
    }

    this.checkVerticalPosition = function(y, marginBothSides, marginOneSide) {
        if (y < marginBothSides) return false;
        if (y + item.height > area.height - marginBothSides) return false;
        if (marginBothSides >= marginOneSide) return true;
        if (y > marginOneSide) return true;
        if (y + item.height < area.height - marginOneSide) return true;
        return false;
    }

    this.checkHorizontalPosition = function(x, marginBothSides, marginOneSide) {
        if (x < marginBothSides) return false;
        if (x + item.width > area.width - marginBothSides) return false;
        if (marginBothSides >= marginOneSide) return true;
        if (x > marginOneSide) return true;
        if (x + item.width < area.width - marginOneSide) return true;
        return false;
    }

    // -1 values are not relevant.

    this.above = function() {
        return area.mapFromItem(caller, -1, 0).y - item.height - callerMargins;
    }

    this.below = function() {
        return area.mapFromItem(caller, -1, caller.height).y + callerMargins;
    }

    this.left = function() {
        return area.mapFromItem(caller, 0, -1).x - item.width - callerMargins;
    }

    this.right = function() {
        return area.mapFromItem(caller, caller.width, -1).x + callerMargins;
    }

    this.horizontalAlign = function() {
        var x = area.mapFromItem(caller, caller.width/2, -1).x - item.width/2;
        return MathUtils.clamp(x, edgeMargins, area.width - item.width - edgeMargins);
    }

    this.verticalAlign = function() {
        var y = area.mapFromItem(caller, -1, caller.height/2).y - item.height/2;
        return MathUtils.clamp(y, edgeMargins, area.height - item.height - edgeMargins);
    }

    this.autoSmallScreenPortrait = function() {
        var coords = new Qt.point(this.horizontalCenter(), 0);
        coords.y = this.above();
        if (this.checkVerticalPosition(coords.y, 0, area.height/4)) {
            return coords;
        }
        coords.y = this.below();
        if (this.checkVerticalPosition(coords.y, 0, area.height/4)) {
            return coords;
        }
        // position at the top of the screen:
        coords.y = 0;
        return coords;
    }

    this.autoSmallScreenLandscape = function() {
        var coords = Qt.point(0, this.verticalCenter());
        coords.x = this.left();
        if (this.checkHorizontalPosition(coords.x, 0, area.width/4)) {
            return coords;
        }
        coords.x = this.right();
        if (this.checkHorizontalPosition(coords.x, 0, area.width/4)) {
            return coords;
        }
        // position at the left of the screen
        coords.x = 0;
        return coords;
    }

    this.autoLargeScreen = function() {
        var coords = Qt.point(0, 0);
        // position with the following priorities: above, left, right, below.
        coords.y = this.above();
        if (this.checkVerticalPosition(coords.y, edgeMargins, 0)) {
            coords.x = this.horizontalAlign();
            return coords;
        }
        coords.x = this.left();
        if (this.checkHorizontalPosition(coords.x, edgeMargins, 0)) {
            coords.y = this.verticalAlign();
            return coords;
        }
        coords.x = this.right();
        if (this.checkHorizontalPosition(coords.x, edgeMargins, 0)) {
            coords.y = this.verticalAlign();
            return coords;
        }
        coords.y = this.below();
        if (this.checkVerticalPosition(coords.y, edgeMargins, 0)) {
            coords.x = this.horizontalAlign();
            return coords;
        }
        // not enough space on any of the sides to fit within the margins.
        coords.x = this.horizontalCenter();
        coords.y = this.verticalCenter();
        return coords;
    }

    this.auto = function() {
        if (item.width >= area.width - 2*edgeMargins) {
            // the popover uses (almost) the full width of the screen
            return this.autoSmallScreenPortrait();
        }
        if (item.height >= area.height - 2*edgeMargins) {
            // the popover uses (almost) the full height of the screen
            return this.autoSmallScreenLandscape();
        }
        return this.autoLargeScreen();
    }
}

