/*
 * Copyright 2016 Canonical Ltd.
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

import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Column {
        DatePicker {
            id: datePicker
            objectName: 'datePicker'
            mode: 'Years|Months|Days'
            maximum: {
                var d = new Date()
                d.setFullYear('2030')
                return d
            }
            minimum: {
                var d = new Date()
                d.setFullYear('1990')
                return d
            }
            date: {
                var d = new Date()
                d.setFullYear('2010')
                d.setMonth('5')
                d.setDate('15')
                return d
            }
        }
        DatePicker {
            id: timePicker
            objectName: 'timePicker'
            mode: 'Hours|Minutes|Seconds'
            maximum: {
                var d = new Date()
                d.setFullYear('2030')
                return d
            }
            minimum: {
                var d = new Date()
                d.setFullYear('1990')
                return d
            }
            date: {
                var d = new Date()
                d.setHours(12)
                d.setMinutes('30')
                d.setSeconds('30')
                return d
            }
        }
    }
}
