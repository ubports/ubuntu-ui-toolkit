#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>

#include "themeengine.h"
#include "themeengine_p.h"
#include "styleditem.h"
#include "style.h"

const char *lookupTestPattern1 =
        "import QtQuick 2.0"
        "import Ubuntu.Components 0.1"
        "StyledItem {"
        "   styleClass: \"testB\""
        "   StyledItem {"
        "       styleClass: \"baseA\""
        "   }"
        "}";

class tst_ThemeEngine : public QObject
{
    Q_OBJECT

public:
    tst_ThemeEngine();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_initializeEngine();
    void testCase_registerInstanceId();
    void testCase_loadTheme();
    void testCase_setTheme();
    void testCase_lookupStyleRule();

private:
    QQuickView *view;
};

tst_ThemeEngine::tst_ThemeEngine():
    view(0)
{
    QCoreApplication *app = QCoreApplication::instance();
    app->setOrganizationName("Canonical");
    app->setApplicationName("UnitTests");
    QSettings settings;
    settings.setValue(appUseGlobalThemeKey, QVariant(false));
    settings.setValue(appThemeFileKey, QVariant("qrc:/base.qthm"));
}

void tst_ThemeEngine::initTestCase()
{
    view = new QQuickView;

    // must register all the types as done in plugin; if used from plugin
    // declarative does not create the Rule objects but QObjects, and does
    // not give any error...

    const char *uri = "Ubuntu.Components";
    qmlRegisterSingletonType<ThemeEngine>(uri, 0, 1, "Theme", ThemeEngine::initializeEngine);
    qmlRegisterType<StyleRule>(uri, 0, 1, "Rule");
    qmlRegisterType<StyledItem>(uri, 0, 1, "StyledItem");
}

void tst_ThemeEngine::cleanupTestCase()
{
    delete view;
}

void tst_ThemeEngine::testCase_initializeEngine()
{
    bool result = (ThemeEngine::initializeEngine(view->engine(), 0) != 0);
    // theme loading might fail, however don't care about it
    QCOMPARE(result, true);
}

void tst_ThemeEngine::testCase_registerInstanceId()
{
    ThemeEngine::instance()->resetError();
    StyledItem *item = new StyledItem(0);
    // first time must pass
    bool result = ThemeEngine::instance()->registerInstanceId(item, "test");
    QCOMPARE(result, true);
    // second time should fail
    result = ThemeEngine::instance()->registerInstanceId(item, "test");
    QCOMPARE(result, false);
    // this should pass always
    result = ThemeEngine::instance()->registerInstanceId(item, QString());
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
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/test.qthm"));
    QCOMPARE(ThemeEngine::instance()->error(), QString());
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/base.qthm"));
    QCOMPARE(ThemeEngine::instance()->error(), QString());
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/null.qthm"));
    QCOMPARE(ThemeEngine::instance()->error().isEmpty(), false);
    ThemeEngine::instance()->loadTheme(QUrl("qrc:/test.qthm"));
    QCOMPARE(ThemeEngine::instance()->error(), QString());
}

void tst_ThemeEngine::testCase_setTheme()
{
    ThemeEngine::instance()->resetError();
    // should pass
    ThemeEngine::instance()->setTheme("../../resources/test.qthm", false);
    QCOMPARE(ThemeEngine::instance()->error(), QString());
    // should fail
    ThemeEngine::instance()->setTheme("../../resources/base.qthm", true);
    QCOMPARE(ThemeEngine::instance()->error().isEmpty(), false);
}


QTEST_MAIN(tst_ThemeEngine)

#include "tst_theme_enginetest.moc"
