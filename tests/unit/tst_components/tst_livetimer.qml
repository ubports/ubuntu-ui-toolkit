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
import Ubuntu.Components 1.3

TestCase {
    name: "LiveTimer"

    function test_0_defaults() {
        compare(liveTimer.frequency, LiveTimer.Disabled, "Default frequency");
    }

    function test_frequency_data() {
        return [
            { tag:"Disabled", frequency: LiveTimer.Disabled },
            { tag:"Second", frequency: LiveTimer.Second },
            { tag:"Minute", frequency: LiveTimer.Minute },
            { tag:"Hour", frequency: LiveTimer.Hour },
            { tag:"Relative", frequency: LiveTimer.Relative },
        ];
    }

    function test_frequency(data) {
        liveTimer.frequency = data.frequency;
        compare(liveTimer.frequency, data.frequency, "LiveTimer frequency can be set.");
    }

    function test_relativeTime() {
        liveTimer.relativeTime = new Date(2015, 0, 0, 0, 0, 0, 0);
        compare(liveTimer.relativeTime, new Date(2015, 0, 0, 0, 0, 0, 0), "Can set/get relativeTime")
    }

    LiveTimer {
        id: liveTimer
    }
}
