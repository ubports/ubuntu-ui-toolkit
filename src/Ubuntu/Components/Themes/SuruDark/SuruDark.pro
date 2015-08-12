TARGETPATH = Ubuntu/Components/Themes/SuruDark

PARENT_THEME_FILE = parent_theme
ARTWORK_FILES +=  artwork/chevron@27.png

QML_FILES += 1.2/MainViewStyle.qml \
             1.2/OptionSelectorStyle.qml \
             1.2/Palette.qml \
             1.2/TabBarStyle.qml \
             1.2/ListItemStyle.qml \
             1.3/OptionSelectorStyle.qml \
             1.3/Palette.qml \
             1.3/TabBarStyle.qml \
             1.3/ListItemStyle.qml \
             $$ARTWORK_FILES \
             $$PARENT_THEME_FILE

load(ubuntu_qml_module)
