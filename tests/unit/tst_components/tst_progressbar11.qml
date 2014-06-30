/*
 * Copyright 2014 Canonical Ltd.
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
import QtTest 1.0
import Ubuntu.Components 1.1

TestCase {
     name: "ProgressBarAPI"

     function test_showProgressPercentageExists() {
         verify(progressBar.hasOwnProperty("showProgressPercentage"), "Property missing")
     }

     function test_noLabel() {
         compare(progressBar.showProgressPercentage,true, "is set by default")
         var newShowProgressPercentage = false
         progressBar.showProgressPercentage = newShowProgressPercentage
         compare(progressBar.showProgressPercentage, newShowProgressPercentage, "can set/get")
     }

     ProgressBar {
         id: progressBar
     }
}
