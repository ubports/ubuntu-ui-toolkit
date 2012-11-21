import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "ModelSectionCounterAPI"

    function test_defaults()
    {
        compare(counter.view, null, "ModelSectionCounter does not have valid view set");
        compare(counter.sectionCount, 0, "ModelSectionCounter default sectionCount is 0");
        compare(counter.cacheSections, false, "ModelSectionCounter does not cache sections");
        compare(counter.sectionCache, [], "ModelSectionCounter has empty section cache list");
    }

    function test_sectionCount()
    {
        counter.view = list;
        compare(counter.view, list, "List not set");
        compare(counter.sectionCount, 10, "Section count is wrong");
    }

    function test_sectionCache()
    {
        counter.view = list;
        counter.cacheSections = true;
        var cache = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"];
        compare(counter.sectionCache, cache, "Section cache is wrong");

        counter.cacheSections = false;
        cache = [];
        compare(counter.sectionCache, cache, "Section cache is wrong");
    }

    Item {
        ListModel {
            id: listModel
            Component.onCompleted: {
                for (var i = 0; i < 10; i++) {
                    listModel.append({"label": i})
                }
            }
        }
        ListView {
            id: list
            model: listModel

            delegate: Item{ height: 40}
            section.property: "label"
            section.criteria: ViewSection.FirstCharacter
        }
    }
    ModelSectionCounter {
        id: counter
    }
}
