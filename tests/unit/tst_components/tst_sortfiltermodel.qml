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
     name: "SortFilterModel"

    ListModel {
        id: things
        ListElement { foo: "pub"; alpha: "bee"; num: 200 }
        ListElement { foo: "den"; alpha: "cow"; num: 300 }
        ListElement { foo: "bar"; alpha: "ant"; num: 100 }
    }

    SortFilterModel {
        id: unmodified
        model: things
    }

    SortFilterModel {
        id: alphabetic
        model: things
        sort.property: "alpha"
    }

    SortFilterModel {
        id: alphaSecond
        model: things
        sort.property: "foo"
    }

    SortFilterModel {
        id: alphabeticRe
        model: things
        sort.property: "alpha"
        sort.order: Qt.DescendingOrder
    }

    SortFilterModel {
        id: numeric
        model: things
        sort.property: "num"
    }

    SortFilterModel {
        id: numericRe
        model: things
        sort.property: "num"
        sort.order: Qt.DescendingOrder
    }

    SortFilterModel {
        id: bee
        model: things
        filter.property: "alpha"
        filter.pattern: /e/
    }

    function test_passthrough() {
        compare(unmodified.count, things.count)
    }

    function test_sort() {
        // Default is Ascending
        compare(alphabetic.sort.order, Qt.AscendingOrder)
        compare(alphabetic.get(0).alpha, "ant")
        compare(alphabetic.get(1).alpha, "bee")
        compare(alphabetic.get(2).alpha, "cow")

        // Ensure different columns work also
        compare(alphaSecond.get(0).foo, "bar")

        // Descending
        compare(alphabeticRe.sort.order, Qt.DescendingOrder)
        compare(alphabeticRe.get(0).alpha, "cow")
        compare(alphabeticRe.get(1).alpha, "bee")
        compare(alphabeticRe.get(2).alpha, "ant")

        // Numbers
        compare(numeric.get(0).num, 100)
        compare(numericRe.get(0).num, 300)

        // Changing roles
        alphabetic.sort.property = "foo"
        compare(alphabetic.get(0).foo, "bar")
        compare(alphabetic.get(1).foo, "den")
        compare(alphabetic.get(2).foo, "pub")
        // Sanity check
        compare(alphabetic.get(1).alpha, "cow")
        // change again
        alphabetic.sort.property = "num"
        compare(alphabetic.get(0).num, 100)
        compare(alphabetic.get(1).num, 200)
        compare(alphabetic.get(2).num, 300)
        // Sanity check
        compare(alphabetic.get(2).alpha, "cow")
    }

    function test_filter() {
        // Default is an empty pattern
        compare(unmodified.filter.pattern, RegExp())

        // Filter
        compare(bee.count, 1)
        compare(bee.get(0).alpha, "bee")
    }
}
