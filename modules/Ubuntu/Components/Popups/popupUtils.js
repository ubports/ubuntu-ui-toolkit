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
}

// TODO: close function.

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
}

