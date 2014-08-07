import QtQuick 2.2
import Ubuntu.Components 1.1

MainView {
    width: units.gu(70)
    height: units.gu(60)
    useDeprecatedToolbar: true

    Tabs {
        id: tabs
        Tab {
            objectName: "tab1"
            title: "Tab1"
            Page {
                head.actions: Action {
                    text: "Test1"
                }
            }
        }
        Tab {
            objectName: "tab2"
            title: "Tab2"
            Page {
                head.actions: Action {
                    text: "Test2"
                }
            }
        }
        Tab {
            objectName: "tab3"
            title: "Tab3"
            Page {
                head.actions: Action {
                    text: "Test3"
                }
            }
        }
    }
}
