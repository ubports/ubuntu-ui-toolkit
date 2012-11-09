import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
     name: "ActivityIndicatorAPI"

     function test_running() {
        compare(activityIndicator.running,false,"Running is boolean and false by default")
        activityIndicator.running = true
        compare(activityIndicator.running,true,"can set/get running")
     }

     ActivityIndicator {
         id: activityIndicator
     }
}
