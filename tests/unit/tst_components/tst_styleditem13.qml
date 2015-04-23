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

import QtTest 1.0
import Ubuntu.Components 1.3
import QtQuick 2.4

TestCase {
     name: "StyledItemAPI"

     StyledItem {
         id: scope
         StyledItem {
             id: inner
         }
     }

     function test_api() {
         verify(scope.theme, "Null default theme");
         compare(inner.theme, scope.theme, "default themes should be inherited!");
     }
}
