import QtQuick 2.2
import Ubuntu.Components 1.1

MainView {
    width: units.gu(70)
    height: units.gu(60)
    useDeprecatedToolbar: false

    Tabs {
        id: tabs
        Tab {
            objectName: "tab1"
            title: "Tab1"
            Page { }
        }
        Tab {
            objectName: "tab2"
            title: "Tab2"
            Page { }
        }
        Tab {
            objectName: "tab3"
            title: "Tab3"
            Page { }
        }
    }
}
