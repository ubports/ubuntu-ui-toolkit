TARGETPATH = Ubuntu/Components/Styles

ARTWORK_FILES = $$system(find artwork -type f)

QML_FILES += 1.2/ComboButtonStyle.qml \
             1.2/PageHeadStyle.qml \
             1.2/PullToRefreshStyle.qml \
             1.3/PageHeadStyle.qml \
	     1.3/ActionBarStyle.qml \
	     1.3/PageHeaderStyle.qml \
             $$ARTWORK_FILES

load(ubuntu_qml_module)

OTHER_FILES+=qmldir
