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

.pragma library
.import "mathUtils.js" as MathUtils

function Positioning(popover, area, caller, edgeMargins, callerMargins) {

    this.above = function() {
        var coords = new Qt.point(0, 0);
        var topCenter = area.mapFromItem(caller, caller.width/2, 0);
        coords.x = topCenter.x - popover.width/2;
        coords.y = topCenter.y - popover.height - callerMargins;
        return coords;
    }

    this.below = function() {
        var coords = new Qt.point(0, 0);
        var bottomCenter = area.mapFromItem(caller, caller.width/2, caller.height);
        coords.x = bottomCenter.x - popover.width/2;
        coords.y = bottomCenter.y + callerMargins;
        return coords;
    }

    this.left = function() {
        var coords = new Qt.point(0, 0);
        var leftCenter = area.mapFromItem(caller, 0, caller.height/2);
        coords.x = leftCenter.x - popover.width - callerMargins;
        coords.y = leftCenter.y - popover.height/2;
        return coords;
    }

    this.right = function() {
        var coords = new Qt.point(0, 0);
        var rightCenter = area.mapFromItem(caller, caller.width, caller.height/2);
        coords.x = rightCenter.x + callerMargins;
        coords.y = rightCenter.y - popover.height/2;
        return coords;
    }

    this.center = function() {
        var coords = new Qt.point(0, 0);
        coords.x = area.width/2 - popover.width/2;
        coords.y = area.height/2 - popover.height/2;
        return coords;
    }

    this.auto = function() {
        var minX = edgeMargins;
        var maxX = area.width - edgeMargins - popover.width;
        var minY = edgeMargins;
        var maxY = area.height - edgeMargins - popover.height;

        var coords = this.above();
        if (coords.y >= minY) {
            coords.x = MathUtils.clamp(coords.x, minX, maxX);
            return coords;
        }

        coords =  this.left();
        if (coords.x >= minX) {
            coords.y = MathUtils.clamp(coords.y, minY, maxY);
            return coords;
        }

        coords = this.right();
        if (coords.x <= maxX) {
            coords.y = MathUtils.clamp(coords.y, minY, maxY);
            return coords;
        }

        coords = this.below();
        if (coords.y <= maxY) {
            coords.x = MathUtils.clamp(coords.x, minX, maxX);
            return coords;
        }
        return this.center();
    }
}

