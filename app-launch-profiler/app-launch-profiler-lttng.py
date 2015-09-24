#!/usr/bin/env python3
# Copyright 2015 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 2.1.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Benjamin Zeller <benjamin.zeller@canonical.com>

import babeltrace
import sys


class MyCounter(dict):
    def __missing__(self, key):
        return 0

if __name__ == '__main__':
    col = babeltrace.TraceCollection()
    if col.add_traces_recursive(sys.argv[1], 'ctf') is None:
        raise RuntimeError('Cannot add trace')

    first_event = -1
    iterations = 0
    events_in_iteration = 2
    numbers = MyCounter()
    minNumbers = MyCounter()
    maxNumbers = MyCounter()

    for event in col.events:
        if (event.name == "app:invokeApplauncher"):
            if (events_in_iteration != 2):
                raise RuntimeError("Wrong Nr of events: " +
                                   str(events_in_iteration))

            events_in_iteration = 1
            first_event = event.timestamp
#           print("Event "+event.name+" occurs after: "+str(0))
            iterations += 1
        else:
            events_in_iteration += 1
            duration = event.timestamp-first_event
            numbers[event.name] += duration

            if minNumbers[event.name] == 0:
                minNumbers[event.name] = duration
            elif minNumbers[event.name] > duration:
                minNumbers[event.name] = duration

            if maxNumbers[event.name] < duration:
                maxNumbers[event.name] = duration

#            print("Event "+event.name+" occurs after: " +
#                  str((event.timestamp-first_event) / 1000 / 1000 / 1000))

    for event in numbers:
        print("---------- Event "+event+" ----------")
        print("Min: " + str(round(minNumbers[event] / 1000 / 1000 / 1000, 4)))
        print("Max: " + str(round(maxNumbers[event] / 1000 / 1000 / 1000, 4)))
        print("Avg: " + str(round(numbers[event]/iterations/1000000/1000, 4)))
