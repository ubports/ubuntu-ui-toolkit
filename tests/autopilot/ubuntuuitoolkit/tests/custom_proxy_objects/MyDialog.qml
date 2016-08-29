import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    objectName: "test_dialog"
    id: testDialog
    title: "Lorem ipsum dolor sit amet"
    text: "Situs vi latin isset abernit"

    Button {
        text: "Confirm"
        onClicked: PopupUtils.close(testDialog)
    }

    Button {
        text: "Cancel"
        onClicked: PopupUtils.close(testDialog)
    }
}
