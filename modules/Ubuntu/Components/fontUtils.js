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

function sizeToPixels(size) {
    var baseSize = units.dp(14)
    return Math.round(modularScale(size) * baseSize)
}

function modularScale(size) {
    switch (size) {
        case "xx-small": return 0.606
        case "x-small": return 0.707
        case "small": return 0.857
        case "medium": return 1.0
        case "large": return 1.212
        case "x-large": return 1.414
        case "xx-large": return 1.714
    }
}
