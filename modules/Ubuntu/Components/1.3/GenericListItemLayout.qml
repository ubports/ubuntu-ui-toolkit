import QtQuick 2.4
//NOTE: USING ROWLAYOUT MEANS GOING FROM 2000ms to about 2400ms in the benchmark
//import QtQuick.Layouts 1.1

Item {
    id: root
    width: parent.width
    height: units.gu(7)

    //FIXME: tapping on the list item divider triggers ListItems' clicked signal!!!

    property alias mainArea: bigMouseArea

    //NOTE: We're exposing containers so that the dev will be able to do firstSlot.onClicked: innerItem.clicked() but
    //StyledItems will eat rightclicks! So if you put a checkbox in a slot, rightclicking on it won't trigger ListItem's
    //contextual menu! We have to change StyledItem to forward right/middleclicks when they're not handled explicity
    //(agreed with zsombi on 20th July)
    //UPDATE: turns out this is actually not the case as of rev1204, you can right click on checkbox and it
    //still open the contextual menu
    property alias firstSlotArea: firstContainer
    property alias secondSlotArea: thirdContainer
    property alias thirdSlotArea: fourthContainer

    property alias firstSlot: firstContainer.children
    property alias secondSlot: thirdContainer.children
    property alias thirdSlot: fourthContainer.children

    property real defaultMargin: units.gu(1)

    property alias title: titleLabel.text
    property alias subtitle: subtitleLabel.text

    MouseArea {
        id: bigMouseArea
        anchors.fill: parent
    }

    Row {
        id: row
        anchors.fill: parent
        spacing: 0

        MouseArea {
            id: firstContainer
            height: parent.height
            //childrenRect.width would trigger a binding loop when used together with children[0].anchors.center = firstContainer
            //width: childrenRect.width + defaultMargin * 2
            width: children[0] ? children[0].width + defaultMargin * 2 : 0
            onChildrenChanged: children[0].anchors.centerIn = firstContainer
        }
        Item {
            id: secondContainer
            //Layout.fillWidth: true
            height: parent.height

            //using this instead of RowLayout saves us a lot of creation time
            width: root.width - firstContainer.width - thirdContainer.width - fourthContainer.width

            Text {
                id: titleLabel
                anchors.top: parent.top
                anchors.topMargin: units.gu(1)
                text: "title"
            }

            Text {
                id: subtitleLabel
                anchors.top: titleLabel.bottom
                anchors.topMargin: units.gu(1)
                text: "one"
            }
        }
        MouseArea {
            id: thirdContainer
            height: parent.height
            width: children[0] ? children[0].width + defaultMargin * 2 : 0
            onChildrenChanged: children[0].anchors.centerIn = thirdContainer
        }
        MouseArea {
            id: fourthContainer
            height: parent.height
            width: children[0] ? children[0].width + defaultMargin * 2 : 0
            onChildrenChanged: children[0].anchors.centerIn = fourthContainer
        }
    }
}
