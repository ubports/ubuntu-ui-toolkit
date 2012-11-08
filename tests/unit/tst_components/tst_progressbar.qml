import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "ProgressBarAPI"

     function test_indeterminate() {
         compare(progressBar.indeterminate,false,"is not set by default")
         var newIndeterminated = true
         progressBar.indeterminate = newIndeterminated
         compare(progressBar.indeterminate,newIndeterminated,"can set/get")
     }

     function test_maximumValue() {
         compare(progressBar.maximumValue,1.0,"is set to 1.0 by default")
         var newMaximumValue = 20.0
         progressBar.maximumValue = newMaximumValue
         compare(progressBar.maximumValue,newMaximumValue,"can set/get")
     }

     function test_minimumValue() {
         compare(progressBar.minimumValue,0,"is set to 0 by default")
         var newMinimumValue = -20.0
         progressBar.minimumValue = newMinimumValue
         compare(progressBar.minimumValue,newMinimumValue,"can set/get")
     }

     function test_value() {
         compare(progressBar.value,0,"is set to 0 by default")
         var newValue = 5
         progressBar.value = newValue
         compare(progressBar.value,newValue,"can set/get")
     }

     ProgressBar {
         id: progressBar
     }
}
