import QtQuick 2.4
import Ubuntu.Components 1.3

StyledItem {
    width: units.gu(40)
    height: units.gu(40)

    ThemeSettings {
        objectName: "suruTheme"
        name: "Ubuntu.Components.Themes.SuruDark"
    }

    StyledItem {
        objectName: "customThemed"
        theme: ThemeSettings {
            name: "CustomTheme"
        }
    }

    StyledItem {
        objectName: "movable"
    }
}
