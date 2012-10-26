#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>

#include "themeengine.h"
#include "themeengine_p.h"
#include "rule.h"
#include "itemstyleattached.h"

class tst_ThemeEngine : public QObject
{
    Q_OBJECT

public:
    tst_ThemeEngine();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_initializeEngine();
    void testCase_registerName();
    void testCase_loadTheme();
    void testCase_lookupStyleRule();

private:
    QQmlEngine *quickEngine;
};

tst_ThemeEngine::tst_ThemeEngine():
    quickEngine(0)
{
}

void tst_ThemeEngine::initTestCase()
{
    quickEngine = new QQmlEngine(this);

    // must register all the types as done in plugin; if used from plugin
    // declarative does not create the Rule objects but QObjects, and does
    // not give any error...

    const char *uri = QString("Ubuntu.Components").toLatin1();
    ThemeEngine::initializeEngine(quickEngine);
    qmlRegisterType<Rule>(uri, 0, 1, "Rule");
    qmlRegisterType<ItemStyleAttached>(uri, 0, 1, "ItemStyle");
}

void tst_ThemeEngine::cleanupTestCase()
{
}

void tst_ThemeEngine::testCase_initializeEngine()
{
    bool result = (ThemeEngine::initializeEngine(quickEngine) != 0);
    // theme loading might fail, however don't care about it
    QCOMPARE(result, true);
}

void tst_ThemeEngine::testCase_registerName()
{
    ThemeEngine::instance()->resetError();
    QQuickItem *item = new QQuickItem(0);
    // first time must pass
    bool result = ThemeEngine::instance()->registerName(item, "test");
    QCOMPARE(result, true);
    // second time should fail
    result = ThemeEngine::instance()->registerName(item, "test");
    QCOMPARE(result, false);
    // this should pass always
    result = ThemeEngine::instance()->registerName(item, QString());
    QCOMPARE(result, true);
    delete item;
}

void tst_ThemeEngine::testCase_lookupStyleRule()
{
    //ThemeEngine::lookupStyleRule requires a complete QML environment therefore its
    // functionality will be tested using its privates
}

void tst_ThemeEngine::testCase_loadTheme()
{
    ThemeEngine::instance()->resetError();
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/test.qmltheme"));
    QCOMPARE(ThemeEngine::instance()->error(), QString());
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/base.qmltheme"));
    QCOMPARE(ThemeEngine::instance()->error(), QString());
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/null.qmltheme"));
    QCOMPARE(ThemeEngine::instance()->error().isEmpty(), false);
    ThemeEngine::instance()->loadTheme(QUrl("qrc:/test.qmltheme"));
    QCOMPARE(ThemeEngine::instance()->error(), QString());
}

QTEST_MAIN(tst_ThemeEngine)

#include "tst_theme_enginetest.moc"
