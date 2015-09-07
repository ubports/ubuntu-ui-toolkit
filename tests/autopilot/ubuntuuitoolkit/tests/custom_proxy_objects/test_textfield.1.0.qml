import QtQuick 2.0
import Ubuntu.Components 1.0

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Item {
        TextField {
            id: simpleTextField
            objectName: "simple_text_field"
        }
        TextField {
            id: textFieldWithoutClearButton
            objectName: "text_field_without_clear_button"
            hasClearButton: false
            anchors.top: simpleTextField.bottom
        }
    }
}
