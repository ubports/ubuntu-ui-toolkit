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

import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 1.1

TestCase {
     name: "UCUnitsAPI"

     function test_gridUnitChanged() {
         signalSpy.signalName = "gridUnitChanged";
         compare(signalSpy.valid,true,"gridUnitChanged signal exists")
     }

     function test_gridUnit() {
         var readValue = units.gridUnit;
         units.gridUnit += 1.0;
         readValue += 1.0;
         compare(units.gridUnit,readValue,"can set/get value");
     }

     function test_gu() {
         var testValue = 1.0;
         var readValue = units.gu(testValue);
         var calculatedValue = Math.round(testValue*units.gridUnit);
         compare(readValue,calculatedValue,"can use units.gu");
     }

     function test_dp() {
         var testValue = 1.0;
         var default_grid_unit_px = 8;
         var readValue = units.dp(testValue);
         var calculatedValue = Math.round(testValue*units.gridUnit / default_grid_unit_px);
         compare(readValue,calculatedValue,"can use units.dp");
     }

     SignalSpy {
         id: signalSpy
         target: units
     }
}
