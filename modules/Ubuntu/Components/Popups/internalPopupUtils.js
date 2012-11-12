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

function Positioning(foreground, pointer, area, caller, edgeMargins, callerMargins) {

    // all coordinate computation are relative inside "area".

    // return the x-coordinate to center item horizontally in area
    this.horizontalCenter = function(item) {
        return area.width/2 - item.width/2;
    }

    // return the y-coordinate to center item vertically in area
    this.verticalCenter = function(item) {
        return area.height/2 - item.height/2;
    }

    // check whether item fits inside area, obeying the given margins
    this.checkVerticalPosition = function(item, y, marginBothSides, marginOneSide) {
        if (y < marginBothSides) return false;
        if (y + item.height > area.height - marginBothSides) return false;
        if (marginBothSides >= marginOneSide) return true;
        if (y > marginOneSide) return true;
        if (y + item.height < area.height - marginOneSide) return true;
        return false;
    }

    // check whether item fits inside area, obeying the given margins
    this.checkHorizontalPosition = function(item, x, marginBothSides, marginOneSide) {
        if (x < marginBothSides) return false;
        if (x + item.width > area.width - marginBothSides) return false;
        if (marginBothSides >= marginOneSide) return true;
        if (x > marginOneSide) return true;
        if (x + item.width < area.width - marginOneSide) return true;
        return false;
    }

    // -1 values are not relevant.

    // return y-coordinate to position item a distance of margin above caller
    this.above = function(item, margin) {
        return area.mapFromItem(caller, -1, 0).y - item.height - margin;
    }

    // return y-coordinate to position item a distance of margin below caller
    this.below = function(item, margin) {
        return area.mapFromItem(caller, -1, caller.height).y + margin;
    }

    // return x-coordinate to position item a distance of margin left of caller
    this.left = function(item, margin) {
        return area.mapFromItem(caller, 0, -1).x - item.width - margin;
    }

    // return x-coodinate to position item a distance of margin right of caller
    this.right = function(item, margin) {
        return area.mapFromItem(caller, caller.width, -1).x + margin;
    }

    // return x-coordinate to align center of item horizontally with center of caller
    this.horizontalAlign = function(item) {
        var x = area.mapFromItem(caller, caller.width/2, -1).x - item.width/2;
        return MathUtils.clamp(x, edgeMargins, area.width - item.width - edgeMargins);
    }

    // return y-coordinate to align center of item vertically with center of caller
    this.verticalAlign = function(item) {
        var y = area.mapFromItem(caller, -1, caller.height/2).y - item.height/2;
        return MathUtils.clamp(y, edgeMargins, area.height - item.height - edgeMargins);
    }

    // position foreground and pointer automatically on a small screen in portrait mode
    this.autoSmallScreenPortrait = function() {
        foreground.x = this.horizontalCenter(foreground);
        if (!caller) {
            foreground.y = 0;
            pointer.direction = "none";
            return;
        }
        var ycoord = this.above(foreground, callerMargins);
        if (this.checkVerticalPosition(foreground, ycoord, 0, area.height/4)) {
            foreground.y = ycoord;
            pointer.direction = "down";
            pointer.y = this.above(pointer, 0);
            pointer.x = this.horizontalAlign(pointer);
            return;
        }
        ycoord = this.below(foreground, callerMargins);
        if (this.checkVerticalPosition(foreground, ycoord, 0, area.height/4)) {
            foreground.y = ycoord;
            pointer.direction = "up";
            pointer.y = this.below(pointer, 0);
            pointer.x = this.horizontalAlign(pointer);
            return;
        }
        // position at the top of the screen:
        foreground.y = 0;
        pointer.direction = "none";
    }

    // position foreground and pointer automatically on a small screen in landscape mode.
    this.autoSmallScreenLandscape = function() {
        foreground.y = this.verticalCenter(foreground);
        if (!caller) {
            foreground.x = 0;
            pointer.direction = "none";
            return;
        }
        var xcoord = this.left(foreground, callerMargins);
        if (this.checkHorizontalPosition(foreground, xcoord, 0, area.width/4)) {
            foreground.x = xcoord;
            pointer.direction = "right";
            pointer.x = this.left(pointer, 0);
            pointer.y = this.verticalAlign(pointer);
            return;
        }
        xcoord = this.right(foreground, callerMargins);
        if (this.checkHorizontalPosition(foreground, xcoord, 0, area.width/4)) {
            foreground.x = xcoord;
            pointer.direction = "left";
            pointer.x = this.right(pointer, 0);
            pointer.y = this.verticalAlign(pointer);
            return;
        }
        // position at the left of the screen
        foreground.x = 0;
        pointer.direction = "none";
    }

    // position foreground and pointer automatically on a large screen.
    this.autoLargeScreen = function() {
        if (!caller) {
            foreground.x = this.horizontalCenter(foreground);
            foreground.y = this.verticalCenter(foreground);
            pointer.direction = "none";
            return;
        }
        // position with the following priorities: above, left, right, below.
        var coord = this.above(foreground, callerMargins);
        if (this.checkVerticalPosition(foreground, coord, edgeMargins, 0)) {
            foreground.y = coord;
            foreground.x = this.horizontalAlign(foreground);
            pointer.direction = "down";
            pointer.y = this.above(pointer, 0);
            pointer.x = this.horizontalAlign(pointer);
            return;
        }
        coord = this.left(foreground, callerMargins);
        if (this.checkHorizontalPosition(foreground, coord, edgeMargins, 0)) {
            foreground.x = coord;
            foreground.y = this.verticalAlign(foreground);
            pointer.direction = "right";
            pointer.x = this.left(pointer, 0);
            pointer.y = this.verticalAlign(pointer);
            return;
        }
        coord = this.right(foreground, callerMargins)
        if (this.checkHorizontalPosition(foreground, coord, edgeMargins, 0)) {
            foreground.x = coord;
            foreground.y = this.verticalAlign(foreground);
            pointer.direction = "left";
            pointer.x = this.right(pointer, 0);
            pointer.y = this.verticalAlign(pointer);
            return;
        }
        coord = this.below(foreground, callerMargins);
        if (this.checkVerticalPosition(foreground, coord, edgeMargins, 0)) {
            foreground.y = coord;
            foreground.x = this.horizontalAlign(foreground);
            pointer.direction = "up";
            pointer.y = this.below(pointer, 0);
            pointer.x = this.horizontalAlign(pointer);
            return;
        }
        // not enough space on any of the sides to fit within the margins.
        coords.x = this.horizontalCenter(foreground);
        coords.y = this.verticalCenter(foreground);
        pointer.direction = "none";
    }

    this.auto = function() {
        if (foreground.width >= area.width - 2*edgeMargins) {
            // the popover uses (almost) the full width of the screen
            this.autoSmallScreenPortrait();
            return;
        }
        if (foreground.height >= area.height - 2*edgeMargins) {
            // the popover uses (almost) the full height of the screen
            this.autoSmallScreenLandscape();
            return;
        }
        this.autoLargeScreen();
    }
}

