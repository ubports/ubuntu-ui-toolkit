#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include <QQmlExtensionPlugin>

class TestPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // TESTPLUGIN_H
