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

     PageStack {
         id: pageStack
     }
}
