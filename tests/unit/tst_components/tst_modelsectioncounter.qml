import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "ModelSectionCounterAPI"

    function test_defaults()
    {
        compare(counter.view, null, "ModelSectionCounter does not have valid view set")
        compare(counter.sectionCount, 0, "ModelSectionCounter default sectionCount is 0")
        compare(counter.cacheSections, false, "ModelSectionCounter does not cache sections")
        compare(counter.sectionCache, [""], "ModelSectionCounter has empty section cache list")
    }

    ListView {
        id: list
        model: ListModel {
            Repeater {
                count: 100
                ListElement {
                    labelRole: index
                }
            }
        }
    }
    ModelSectionCounter {
        id: counter
    }
}
