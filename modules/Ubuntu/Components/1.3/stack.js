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

.pragma library
// By defining Stack as a function, we can make its variables private,
// and force calls to Stack to make use of the functions we define.
function Stack() {
    var elements;
    this.clear = function() {
        elements = [];
    }

    this.clear();

    this.push = function(element) {
        elements.push(element);
    };

    this.pop = function() {
        elements.pop();
    };

    this.size = function() {
        return elements.length;
    }

    this.top = function() {
        if (this.size() < 1) return undefined;
        return elements[elements.length-1];
    }

    // returns the topmost element for the column, using exact column match
    // in case the column to be extracted is less than the columns available
    this.topForColumn = function(column, exactMatch) {
        if (exactMatch == undefined) {
            exactMatch = false;
        }

        for (var i = elements.length - 1; i >= 0; i--) {
            var node = elements[i];
            if ((exactMatch && elements[i].column == column) || (!exactMatch && elements[i].column >= column)) {
                return elements[i];
            }
        }
        return undefined;
    }

    // returns the node the Page is stored; undefined if not found
    this.find = function(page) {
        if (!page) return null;
        for (var i = elements.length - 1; i >= 0; i--) {
            if (elements[i].object == page) {
                return elements[i];
            }
        }
        return null;
    }
}
