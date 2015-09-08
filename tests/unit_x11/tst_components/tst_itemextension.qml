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

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Component {
        id: component
        Column {
            Repeater {
                model: 4
                Item {
                    width: units.gu(10)
                    height: units.gu(2)
                }
            }
        }
    }

    Column {
        id: column
        anchors.fill: parent
        Loader {
            id: loader
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "ItemExtension"
        when: windowShown

        SignalSpy {
            id: parentChangedSpy
            signalName: "parentChanged"
        }
        SignalSpy {
            id: extendedParentChangedSpy
            signalName: "extendedParentChanged"
        }

        function initTestCase() {
            extendedParentChangedSpy.target = column;
            verify(extendedParentChangedSpy.valid, "Column not extended");
            extendedParentChangedSpy.target = loader;
            extendedParentChangedSpy.clear();
            verify(extendedParentChangedSpy.valid, "Loader not extended");
        }

        function cleanup() {
            extendedParentChangedSpy.target = null;
            parentChangedSpy.target = null;
            extendedParentChangedSpy.clear();
            parentChangedSpy.clear();
            loader.sourceComponent = null;
        }

        function test_parentChanged_emitted() {
            loader.sourceComponent = component;
            tryCompareFunction(function() {return loader.status == Loader.Ready}, true, 1000);
            parentChangedSpy.target = loader.item;
            loader.item.parent = main;
            parentChangedSpy.wait();
        }
    }
}
