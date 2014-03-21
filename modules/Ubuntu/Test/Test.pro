TEMPLATE = subdirs
SUBDIRS += plugin
OTHER_FILES += $$system(ls *.qml)
include(deployment.pri)

