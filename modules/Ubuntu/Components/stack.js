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

// By defining Stack as a function, we can
// make its variables private, and force calls
// to Stack to make use of the functions we define.
function Stack() {
    var elements; // = new Array();
    this.clear = function() {
        elements = [];
        this.size = 0;
        this.top = undefined;
    }

//    this.clear();

    this.push = function(element) {
             elements.push(element);
             this.size++;
             this.top = element;
         };

    this.pop = function() {
             elements.pop();
             this.size--;
             this.top = elements[this.size-1];
         };
}

// needed because I cannot create a JS as a property in QML1
var stack = new Stack();
