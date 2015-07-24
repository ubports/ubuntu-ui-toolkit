TARGETPATH = Ubuntu/Components/Themes


ARTWORK_FILES = $$system(find artwork -type f)

QML_FILES += 1.2/Palette.qml \
             1.2/PaletteValues.qml \
             1.3/Palette.qml \
             1.3/PaletteValues.qml \
             $$ARTWORK_FILES

load(ubuntu_qml_module)
