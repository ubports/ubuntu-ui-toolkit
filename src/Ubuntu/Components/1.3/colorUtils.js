/*
 * Copyright (C) 2013-2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*!
  \internal
  \qmltype colorUtils
  \inqmlmodule Ubuntu.Components 1.3
  \ingroup ubuntu
  \brief Various chromatic utility functions.
 */

.pragma library

/*!
  \qmlmethod colorUtils::luminance(hexcolor)
  Get the luminance for a color.
 */
function luminance(hexcolor){
    hexcolor = String(hexcolor)
    var r = parseInt(hexcolor.substr(1,2),16);
    var g = parseInt(hexcolor.substr(3,2),16);
    var b = parseInt(hexcolor.substr(5,2),16);
    return ((r*212)+(g*715)+(b*73))/1000/255;
}
