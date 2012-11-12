import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "PageAPI"

     function test_title() {
         compare(page.title,"","is not set by default")
         var newTitle = "Hello World!"
         page.title = newTitle
         compare(page.title,newTitle,"can set/get")
     }

     function test_pageStack() {
         compare(page.pageStack,null,"is not set by default")
     }

     Page {
         id: page
     }
}
