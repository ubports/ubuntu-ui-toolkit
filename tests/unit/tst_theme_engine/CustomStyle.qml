import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    width: 100
    height: 62

    Item {
        objectName: "item1"
        // test item reflecting button properties styled
        property color color
        property url iconSource
        property string text
        property string iconPosition

        Item {
            objectName: "item2"
            // test item reflecting button properties styled
            property color color
            property url iconSource
            property string text
            property string iconPosition
        }
    }
}
