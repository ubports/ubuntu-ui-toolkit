#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>

#include "themeengine.h"
#include "themeengine_p.h"
#include "styleditem.h"
#include "style.h"

class ThemeEnginePrivateTest : public QObject
{
    Q_OBJECT
    
public:
    ThemeEnginePrivateTest();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase_loadTheme();
    void testCase_styleRuleForPath();
    void testCase_parseSelector();
    void testCase_selectorToString();
private:
    QQuickView *view;
    ThemeEnginePrivate *engine;
    friend class ThemeEnginePrivate;
};

ThemeEnginePrivateTest::ThemeEnginePrivateTest()
{
    QCoreApplication *app = QCoreApplication::instance();
    app->setOrganizationName("Canonical");
    app->setApplicationName("UnitTests");
    QSettings settings;
    settings.setValue(appUseGlobalThemeKey, QVariant(false));
    settings.setValue(appThemeFileKey, QVariant(""));
}


void ThemeEnginePrivateTest::initTestCase()
{
    view = new QQuickView();

    // must register all the types as done in plugin; if used from plugin
    // declarative does not create the Rule objects but QObjects, and does
    // not give any error...

    char *uri = "Ubuntu.Components";
    qmlRegisterSingletonType<ThemeEngine>(uri, 0, 1, "Theme", ThemeEngine::registerEngine);
    qmlRegisterType<StyleRule>(uri, 0, 1, "Rule");
    qmlRegisterType<StyledItem>(uri, 0, 1, "StyledItem");

    // engine privates can be created with its public class; therefore
    // we create an engine class and use its privates
    engine = ThemeEngine::instance(view->engine())->d_func();
}

void ThemeEnginePrivateTest::cleanupTestCase()
{
    delete view;
}

void ThemeEnginePrivateTest::testCase_loadTheme()
{
    engine->errorString = QString();
    engine->loadTheme(QUrl::fromLocalFile("../resources/test.qthm"));
    QVERIFY2(engine->errorString.isEmpty(), "FAILURE");

    engine->loadTheme(QUrl::fromLocalFile("../resources/base.qthm"));
    QVERIFY2(engine->errorString.isEmpty(), "FAILURE");

    engine->loadTheme(QUrl::fromLocalFile("../resources/null.qthm"));
    QVERIFY2(!engine->errorString.isEmpty(), "FAILURE");
}

void ThemeEnginePrivateTest::testCase_styleRuleForPath()
{
    engine->errorString = QString();
    engine->loadTheme(QUrl::fromLocalFile("../resources/test.qthm"));
    QVERIFY2(engine->errorString.isEmpty(), "FAILURE");

    bool result = true;
    StyleRule *rule;
    Selector path, expected;

    path << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (engine->parseSelector(rule->selector())[0] == path);
    QVERIFY2(result, "Failure");

    path.clear();
    path << SelectorNode("testA", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (engine->parseSelector(rule->selector())[0] == path);
    QVERIFY2(result, "Failure");

    path.clear();
    path << SelectorNode("testA", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Child);
    expected << SelectorNode("testA", "", SelectorNode::Descendant);
    expected << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should pass, but should be ".testA .baseA"
    result = (rule != 0) && (engine->parseSelector(rule->selector())[0] == expected);
    QVERIFY2(result, "Failure");

    path.clear();
    path << SelectorNode("testB", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Child);
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (engine->parseSelector(rule->selector())[0] == path);
    QVERIFY2(result, "Failure");

    path.clear();
    path << SelectorNode("testB", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should fail
    result = (rule != 0) && !(engine->parseSelector(rule->selector())[0] == path);
    QVERIFY2(result, "Failure");
}

void ThemeEnginePrivateTest::testCase_parseSelector()
{
    engine->errorString = QString();

    // build selector path
    QList<Selector> selectors = engine->parseSelector(".testB .baseA");
    Selector expected, expected2;
    expected << SelectorNode("testB", "", SelectorNode::Descendant);
    expected << SelectorNode("baseA", "", SelectorNode::Descendant);
    // should match
    bool result = (selectors.count() == 1) && (selectors[0] == expected);
    QVERIFY2(result, "FAILURE");

    selectors = engine->parseSelector(".testB > .baseA");
    // should not match
    result = (selectors.count() == 1) && !(selectors[0] == expected);
    QVERIFY2(result, "FAILURE");

    expected.clear();
    expected << SelectorNode("root", "id", SelectorNode::Descendant);
    expected << SelectorNode("testB", "", SelectorNode::Child);
    expected << SelectorNode("baseB", "", SelectorNode::Descendant);
    selectors = engine->parseSelector(".root#id .testB > .baseA");
    // should not match!
    result = (selectors.count() == 1) && !(selectors[0] == expected);
    QVERIFY2(result, "FAILURE");

    selectors = engine->parseSelector(".root#id > .testB .baseB");
    // should match
    result = (selectors.count() == 1) && (selectors[0] == expected);
    QVERIFY2(result, "FAILURE");

    selectors = engine->parseSelector(".root#id > .testB .baseB, .oneNode.bing .baseC");
    expected2 << SelectorNode("oneNode.bing", "", SelectorNode::Descendant);
    expected2 << SelectorNode("baseC", "", SelectorNode::Descendant);
    result = (selectors.count() == 2) &&
            (selectors[0] == expected) &&
            (selectors[1] == expected2);
    QVERIFY2(result, "FAILURE");
}

void ThemeEnginePrivateTest::testCase_selectorToString()
{
    engine->errorString = QString();
    bool result = true;
    Selector selector;
    QString expected;

    selector.clear();
    selector << SelectorNode("classA", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "", SelectorNode::Descendant);
    expected = ".classA .classB";
    result = engine->selectorToString(selector) == expected;
    QVERIFY2(result, "Failure");

    selector.clear();
    selector << SelectorNode("classA", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "", SelectorNode::Child);
    expected = ".classA > .classB";
    result = engine->selectorToString(selector) == expected;
    QVERIFY2(result, "Failure");

    selector.clear();
    selector << SelectorNode("classA", "id", SelectorNode::Descendant);
    selector << SelectorNode("classB", "", SelectorNode::Descendant);
    expected = ".classA#id .classB";
    result = engine->selectorToString(selector) == expected;
    QVERIFY2(result, "Failure");

    selector.clear();
    selector << SelectorNode("classA", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "id", SelectorNode::Child);
    expected = ".classA > .classB#id";
    result = engine->selectorToString(selector) == expected;
    QVERIFY2(result, "Failure");

    selector.clear();
    selector << SelectorNode("classA.attribute", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "id", SelectorNode::Child);
    expected = ".classA.attribute > .classB#id";
    result = engine->selectorToString(selector) == expected;
    QVERIFY2(result, "Failure");
}

QTEST_MAIN(ThemeEnginePrivateTest)

#include "tst_themeengineprivatetest.moc"
