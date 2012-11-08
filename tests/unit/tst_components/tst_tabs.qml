import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "TabsAPI"

     function test_buttonsExpanded() {
        compare(tabs.buttonsExpanded,false,"buttonsExpanded is false by default")
        var newButtonsExpanded = true
        tabs.buttonsExpanded = newButtonsExpanded
        compare(tabs.buttonsExpanded,newButtonsExpanded,"Can set/get")
     }

     Tabs {
         id: tabs
     }
}
