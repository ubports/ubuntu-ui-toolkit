import QtQuick 2.4

MouseArea {
    id: container

    property string position: "Slot.Trailing"
    property bool handleInnerItemPosition: true

    //parent is not defined when first assigned to SlotsLayout's "slots" property
    implicitHeight: parent ? parent.height : 0

    //FIXME: hardcoded 2gu margin
    width: children[0] ? children[0].width + units.gu(2) * 2 : 0

    //center the FIRST children (assumption made to avoid overcomplicating logic)
    Binding {
        target: children[0]
        when: children[0] && handleInnerItemPosition
        property: "anchors.centerIn"
        value: container
    }
}
