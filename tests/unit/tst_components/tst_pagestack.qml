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
import Ubuntu.Components 0.1

TestCase {
     name: "PageStackAPI"

     function test_depth() {
         compare(pageStack.depth,0,"depth is 0 by default")
     }

     function test_currentPage() {
         compare(pageStack.currentPage,null,"is not set by default")
     }

     function test_pushAndPopOne() {
         pageStack.push(page1);
         compare(pageStack.currentPage, page1, "was pushed");
         compare(pageStack.depth, 1, "One page was pushed");
         pageStack.pop();
         compare(pageStack.currentPage, null, "was popped");
         compare(pageStack.depth, 0, "one page was pushed and popped");
     }

     function test_pushAndPopTwo() {
         pageStack.push(page1);
         pageStack.push(page2);
         compare(pageStack.currentPage, page2, "was pushed");
         compare(pageStack.depth, 2, "Two pages were pushed");
         pageStack.pop();
         compare(pageStack.currentPage, page1, "was pushed");
         compare(pageStack.depth, 1, "one page left");

         pageStack.clear();
         test_depth();
         test_currentPage();
     }

     PageStack {
         id: pageStack
     }

     Page {
         id: page1
     }

     Page {
         id: page2
     }
}
