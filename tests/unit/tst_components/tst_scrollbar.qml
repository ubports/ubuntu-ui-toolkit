import QtQuick 2.0
import QtTest 1.0
import Ubuntu.Components 0.1

TestCase {
    name: "ScrollbarAPI"

    function initTestCase() {
        //Theme.loadTheme(Qt.resolvedUrl("../../../themes/Ambiance/qmltheme/default.qmltheme"))
        Theme.loadTheme(Qt.resolvedUrl("../../resources/components.qmltheme"))
        //compare(Theme.currentTheme, "", "Theme loading failure");
        compare(Theme.error, "", "Theme loading failure");
        //compare(signals.count,1,"style was changed");
    }

    function test_a()
    {
        compare(scrollbar.flickableItem, null, "Scrollbar does not have flickableItem set by default")
        var defaultAlign = Qt.AlignRight
        compare(scrollbar.align, defaultAlign, "Scrollbar is aligned to the right by default")
    }

    function test_flickableItem_Flickable()
    {
        scrollbar.flickableItem = flickable
        compare(scrollbar.flickableItem, flickable, "Cannot get/set flickableItem property")
    }

    function test_flickableItem_ListView()
    {
        scrollbar.flickableItem = listView
        compare(scrollbar.flickableItem, listView, "Cannot get/set flickableItem property")
    }

    function test_align_left()
    {
        scrollbar.align = Qt.AlignLeft
        compare(scrollbar.align, Qt.AlignLeft, "Cannot get/set align property")
    }

    function test_align_top()
    {
        scrollbar.align = Qt.AlignTop
        compare(scrollbar.align, Qt.AlignTop, "Cannot get/set align property")
    }

    function test_align_right()
    {
        scrollbar.align = Qt.AlignRight
        compare(scrollbar.align, Qt.AlignRight, "Cannot get/set align property")
    }

    function test_align_bottom()
    {
        scrollbar.align = Qt.AlignBottom
        compare(scrollbar.align, Qt.AlignBottom, "Cannot get/set align property")
    }

    function test_nonFlickable() {
        var failed = false
        try {
            scrollbar.flickableItem = nonFlickable
        } catch (e) {
            failed = true
        } finally {
            compare(failed,true,"can not set non-flickable item to flickableItem")
        }
    }

    function test_scrollbar_height_with_ListView_data() {
        return [
            {tag: "Empty model", model: emptyModel, expect: 0, expectFail: false},
            {tag: "Empty model + header", model: emptyModel, header: headerFooter, expect: 20, expectFail: false},
            {tag: "Empty model + header + footer", model: emptyModel, header: headerFooter, footer: headerFooter, expect: 40, expectFail: false},
            {tag: "Empty model + header + footer + section (inline)", model: emptyModel, header: headerFooter, footer: headerFooter, section: section, expect: 40, expectFail: false},
            {tag: "Empty model + header + footer + section (top)", model: emptyModel, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart, expect: 40, expectFail: false},
            {tag: "Empty model + header + footer + section (bottom)", model: emptyModel, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.NextLabelAtEnd, expect: 40, expectFail: false},
            {tag: "Empty model + header + footer + section (top+bottom)", model: emptyModel, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart | ViewSection.NextLabelAtEnd, expect: 40, expectFail: false},

            {tag: "ObjectList model", model: objectList, expect: 150, expectFail: false},
            {tag: "ObjectList model + header", model: objectList, header: headerFooter, expect: 170, expectFail: false},
            {tag: "ObjectList model + header + footer", model: objectList, header: headerFooter, footer: headerFooter, expect: 190, expectFail: false},
            {tag: "ObjectList model + header + footer + section (inline)", model: objectList, header: headerFooter, footer: headerFooter, section: section, expect: 290, expectFail: true},
            {tag: "ObjectList model + header + footer + section (top)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart, expect: 190, expectFail: false},
            {tag: "ObjectList model + header + footer + section (bottom)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.NextLabelAtEnd, expect: 190, expectFail: false},
            {tag: "ObjectList model + header + footer + section (top+bottom)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart | ViewSection.NextLabelAtEnd, expect: 190, expectFail: false},

            {tag: "VariantList model", model: objectList, expect: 150, expectFail: false},
            {tag: "VariantList model + header", model: objectList, header: headerFooter, expect: 170, expectFail: false},
            {tag: "VariantList model + header + footer", model: objectList, header: headerFooter, footer: headerFooter, expect: 190, expectFail: false},
            {tag: "VariantList model + header + footer + section (inline)", model: objectList, header: headerFooter, footer: headerFooter, section: section, expect: 290, expectFail: true},
            {tag: "VariantList model + header + footer + section (top)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart, expect: 190, expectFail: false},
            {tag: "VariantList model + header + footer + section (bottom)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.NextLabelAtEnd, expect: 190, expectFail: false},
            {tag: "VariantList model + header + footer + section (top+bottom)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart | ViewSection.NextLabelAtEnd, expect: 190, expectFail: false},

            {tag: "StringList model", model: objectList, expect: 150, expectFail: false},
            {tag: "StringList model + header", model: objectList, header: headerFooter, expect: 170, expectFail: false},
            {tag: "StringList model + header + footer", model: objectList, header: headerFooter, footer: headerFooter, expect: 190, expectFail: false},
            {tag: "StringList model + header + footer + section (inline)", model: objectList, header: headerFooter, footer: headerFooter, section: section, expect: 290, expectFail: true},
            {tag: "StringList model + header + footer + section (top)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart, expect: 190, expectFail: false},
            {tag: "StringList model + header + footer + section (bottom)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.NextLabelAtEnd, expect: 190, expectFail: false},
            {tag: "StringList model + header + footer + section (top+bottom)", model: objectList, header: headerFooter, footer: headerFooter, section: section, sectionPositioning: ViewSection.CurrentLabelAtStart | ViewSection.NextLabelAtEnd, expect: 190, expectFail: false},
        ];
    }

    function test_scrollbar_height_with_ListView(data) {
        if (data.expectFail)
            expectFail("", data.tag);

        // set data
        listView.model = data["model"] ? data.model : null;
        listView.header = data["header"] ? data.header : null;
        listView.footer = data["footer"] ? data.footer : null;
        listView.section.property = "label";
        listView.section.criteria = ViewSection.FirstCharacter;
        listView.section.labelPositioning = data["sectionPositioning"] ? data.sectionPositioning : ViewSection.InlineLabels;
        listView.section.delegate = data["section"] ? data.section : null;
        scrollbar.flickableItem = listView;

        wait(200);

        compare(scrollbar.ItemStyle.delegate.contentSize, data.expect, data.tag);
    }

    Rectangle {
       id: nonFlickable
    }

    Flickable{
        id: flickable
    }

    ListView {
        id: listView
        delegate: Item {
            height: 15
        }
    }

    Scrollbar {
        id: scrollbar
        SignalSpy {
            id: signals
            target: scrollbar.ItemStyle
            signalName: "styleChanged"
        }
    }

    Component {
        id: headerFooter
        Item { height: 20}
    }

    Component {
        id: section
        Item { height: 10}
    }

    property var emptyModel: ListModel {}

    property var objectList: ListModel {
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

    property var variantList: [
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

    property var stringList: ["1", "10", "100", "2", "3", "4", "5", "6", "7", "8", "9"]
}
