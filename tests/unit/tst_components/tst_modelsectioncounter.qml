import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "ModelSectionCounterAPI"

    function initTestCase() {
        var component = Qt.createComponent(Qt.resolvedUrl("../../../modules/Ubuntu/Components/ModelSectionCounter.qml"));
        counter = component.createObject(parent);
    }

    function test_0_defaults() {
        compare(counter.view, null, "ModelSectionCounter does not have valid view set");
        compare(counter.count, 0, "ModelSectionCounter default sectionCount is 0");
        compare(counter.cacheSections, false, "ModelSectionCounter does not cache sections");
        compare(counter.cache, [], "ModelSectionCounter has empty section cache list");
    }

    function test_1_count_data() {
        return [
            {tag: "Empty model", model: emptyModel, sectionProperty: "label", expect: 0, expectFail: false},
            {tag: "ListModel", model: objectModel, sectionProperty: "label", expect: 8, expectFail: false},
            {tag: "StringList model", model: stringModel, sectionProperty: "modelData", expect: 8, expectFail: false},
            {tag: "VariantList model", model: variantModel, sectionProperty: "label", expect: 8, expectFail: false},
        ];
    }

    function test_1_count(data) {
        if (data.expectFail)
            expectFail("", data.tag);
        list.model = data.model;
        list.section.property = data.sectionProperty;
        list.section.criteria = ViewSection.FirstCharacter;
        counter.view = list;
        compare(counter.count, data.expect, "Section count is wrong");
    }

    function test_2_cache() {
        list.model = objectModel;
        list.section.property = "label";
        list.section.criteria = ViewSection.FirstCharacter;
        counter.view = list;
        counter.cacheSections = true;
        var cache = ["1", "2", "3", "4", "5", "6", "7", "8"];
        compare(counter.cache, cache, "Section cache is wrong");

        counter.cacheSections = false;
        cache = [];
        compare(counter.cache, cache, "Section cache is wrong");
    }

    Item {
        ListView {
            id: list

            delegate: Item{ height: 40 }
            section.delegate: Item{ height: 10 }
        }
    }

    property var counter

    property var emptyModel: ListModel {}

    property var objectModel: ListModel {
        ListElement {label: "1"}
        ListElement {label: "10"}
        ListElement {label: "100"}
        ListElement {label: "2"}
        ListElement {label: "3"}
        ListElement {label: "4"}
        ListElement {label: "5"}
        ListElement {label: "6"}
        ListElement {label: "7"}
        ListElement {label: "8"}
    }

    property var variantModel: [
        {"label": "1"},
        {"label": "10"},
        {"label": "100"},
        {"label": "2"},
        {"label": "3"},
        {"label": "4"},
        {"label": "5"},
        {"label": "6"},
        {"label": "7"},
        {"label": "8"}
    ]

    property var stringModel: ["1", "10", "100", "2", "3", "4", "5", "6", "7", "8"]
}
