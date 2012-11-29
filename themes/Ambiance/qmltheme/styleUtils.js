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

/* checks whether the property is defined in the itemStyle, and if found, returns
  the property value or the given (optional) default value.
  Example:
     itemStyleProperty("color", Qt.rgba(0,0,0,0))
     itemStyleProperty("background") - returns "undefined" if the background
        property is not found.
 */
function itemStyleProperty(property, defaultValue) {
    if ('undefined' !== typeof itemStyle[property])
        return itemStyle[property];
    return defaultValue;
}
