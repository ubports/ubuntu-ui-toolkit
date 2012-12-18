import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

PageStack {
    id: pageStack

    width: units.gu(38)

    Component.onCompleted: push(mainPage)

    function loadApp(app,resources) {
        var component = Qt.createComponent(app);
        if (component.status === Component.Ready) {
            var obj = component.createObject(pageStack);
            obj.rootPath = resources;
            pageStack.push(obj);
        } else {

        }

    }

    Page {
        id: mainPage
        title: i18n.tr("Examples")

        Column {
            anchors.fill: parent
            ListItem.Subtitled {
                text: i18n.tr("uChuck")
                subText: i18n.tr("Label, Button, JSON, SoundEffect")
                onClicked: {
                    loadApp("uchuck/uchuck.qml","../resources/");
                }
                progression: true
            }
        }
    }
}
