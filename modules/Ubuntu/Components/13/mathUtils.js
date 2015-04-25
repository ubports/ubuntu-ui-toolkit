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

// Ensure the value x is between min and max
function clamp(x, min, max) {
    if (min <= max) {
        return Math.max(min, Math.min(x, max));
    } else {
        // swap min/max if min > max
        return clamp(x, max, min);
    }
}

// Get the linear interpolation
function lerp(delta, from, to) {
    return ((1.0 - delta) * from) + (delta * to);
}

// Linearly project a value x from [xmin, xmax] into [ymin, ymax]
function projectValue(x, xmin, xmax, ymin, ymax) {
    return ((x - xmin) * ymax - (x - xmax) * ymin) / (xmax - xmin)
}

// Linearly project a value x, but in addition to projectValue it's clamped to xmin/xmax first
function clampAndProject(x, xmin, xmax, ymin, ymax) {
    return projectValue(clamp(x, xmin, xmax), xmin, xmax, ymin, ymax)
}
