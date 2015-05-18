TARGETPATH = Ubuntu/Component/Popups

ARTWORK_FILES = $$system(find artwork -type f)

QML_FILES += 1.2/ComboButtonStyle.qml \
             1.2/PageHeadStyle.qml \
             1.2/PullToRefreshStyle.qml \
             $$ARTWORK_FILES

load(qml_module)




