import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    id: root

    property string themeError: Theme.error
    property string themeFile: Theme.currentTheme

    Item {
        id: testA
        ItemStyle.class: "testA"
        Item {
            id: baseA
            ItemStyle.class: "baseA"
        }
    }
}
