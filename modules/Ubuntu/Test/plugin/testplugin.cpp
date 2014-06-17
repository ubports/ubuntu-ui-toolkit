#include "testplugin.h"
#include <QtQml>
#include "uctestextras.h"

static QObject *registerExtras(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new TestExtras;
}

void TestPlugin::registerTypes(const char *uri)
{
    qmlRegisterSingletonType<TestExtras>(uri, 1, 0, "TestExtras", registerExtras);
}
