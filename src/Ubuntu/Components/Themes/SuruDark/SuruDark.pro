TARGETPATH = Ubuntu/Component/Themes/SuruDark

PARENT_THEME_FILE = parent_theme
DEPRECATED_FILE = deprecated
ARTWORK_FILES +=  artwork/chevron@27.png

QML_FILES += 1.2/MainViewStyle.qml \
             1.2/OptionSelectorStyle.qml \
             1.2/Palette.qml \
             1.2/TabBarStyle.qml \
             1.3/MainViewStyle.qml \
             1.3/OptionSelectorStyle.qml \
             1.3/Palette.qml \
             1.3/TabBarStyle.qml \
             $$ARTWORK_FILES \
             $$PARENT_THEME_FILE \
             $$DEPRECATED_FILE

load(qml_module)
