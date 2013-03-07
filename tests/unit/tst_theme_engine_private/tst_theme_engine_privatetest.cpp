/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include "themeengine.h"
#include "themeengine_p.h"
#include "itemstyleattached.h"
#include "stylecache_p.h"

#define QVERIFY2_RET(statement, description) \
do {\
    if (statement) {\
        if (!QTest::qVerify(true, #statement, (description), __FILE__, __LINE__))\
            return false;\
    } else {\
        if (!QTest::qVerify(false, #statement, (description), __FILE__, __LINE__))\
            return false;\
    }\
} while (0)

#define QVERIFY_RET(statement) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return false;\
} while (0)

#define QCOMPARE_RET(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return false;\
} while (0)


class tst_ThemeEnginePrivate : public QObject
{
    Q_OBJECT

public:
    tst_ThemeEnginePrivate();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase_registerName();
    void testCase_loadTheme();
    void testCase_urlMacro();
    void testCase_styleRuleForPath();
    void testCase_parseSelector();
    void testCase_selectorToString();
    void testCase_inheritance();
private:
    bool test_styleProperties(const QString &styleClass, const QString &propertyList, bool xfail);
private:
    QQmlEngine *quickEngine;
    ThemeEnginePrivate *engine;
    friend class ThemeEnginePrivate;
};

tst_ThemeEnginePrivate::tst_ThemeEnginePrivate():
    quickEngine(0),
    engine(0)
{
}


void tst_ThemeEnginePrivate::initTestCase()
{
    quickEngine = new QQmlEngine(this);

    // must register all the types as done in plugin; if used from plugin
    // declarative does not create the Rule objects but QObjects, and does
    // not give any error...

    ThemeEngine::initializeEngine(quickEngine);

    // engine privates can be created with its public class; therefore
    // we create an engine class and use its privates
    engine = ThemeEngine::instance()->d_func();
}

void tst_ThemeEnginePrivate::cleanupTestCase()
{
}

void tst_ThemeEnginePrivate::testCase_registerName()
{
    ThemeEngine::instance()->resetError();
    QQuickItem *item = new QQuickItem(0);
    // first time must pass
    bool result = ThemeEnginePrivate::registerName(item, "test");
    QCOMPARE(result, true);
    // second time should fail
    result = ThemeEnginePrivate::registerName(item, "test");
    QCOMPARE(result, false);
    // this should pass always
    result = ThemeEnginePrivate::registerName(item, QString());
    QCOMPARE(result, true);
    delete item;
}

void tst_ThemeEnginePrivate::testCase_loadTheme()
{
    engine->errorString = QString();
    engine->loadTheme(QUrl::fromLocalFile("../../resources/test.qmltheme"));
    QCOMPARE(engine->errorString, QString());

    engine->loadTheme(QUrl::fromLocalFile("../../resources/base.qmltheme"));
    QCOMPARE(engine->errorString, QString());

    engine->loadTheme(QUrl::fromLocalFile("../../resources/null.qmltheme"));
    QCOMPARE(engine->errorString.isEmpty(), false);
}

void tst_ThemeEnginePrivate::testCase_urlMacro()
{
    QUrl themeFile = QUrl::fromLocalFile("../../resources/urlmacro.qmltheme");
    QFileInfo fi(themeFile.path());
    // the URLs in the theme are relative to the theme file
    QDir dir(fi.dir());

    engine->errorString.clear();
    engine->loadTheme(themeFile);

    Selector selector = engine->parseSelector(".baseA")[0];
    StyleCache::StyleData *rule = engine->styleRuleForPath(selector);
    QVERIFY2(rule, "Failure");
    if (rule) {
        // create style from the rule so we can check the validity of the URLs
        QObject *style = rule->style ? rule->style->create(quickEngine->rootContext()) : 0;
        QVERIFY2(style, "FAILURE");

        QString url = style->property("prop_baseA_A").toString();
        QFileInfo fi("../../resources/base.qmltheme");
        QCOMPARE(url, fi.absoluteFilePath());

        url = style->property("prop_baseA_B").toString();
        QCOMPARE(url, fi.absoluteFilePath());

        url = style->property("prop_baseA_C").toString();
        QCOMPARE(url, fi.absoluteFilePath());

        // resources should be the same!
        url = style->property("prop_baseA_D").toString();
        QString url2 = style->property("prop_baseA_E").toString();
        QCOMPARE(url, url2);

        url = style->property("prop_baseA_F").toString();
        url2 = style->property("prop_baseA_G").toString();
        QCOMPARE(url, url2);

        url = style->property("prop_baseA_H").toString();
        url2 = style->property("prop_baseA_I").toString();
        QCOMPARE(url, url2);

        style->deleteLater();
    }
}

void tst_ThemeEnginePrivate::testCase_styleRuleForPath()
{
    engine->errorString = QString();
    engine->loadTheme(QUrl::fromLocalFile("../../resources/test.qmltheme"));
    QCOMPARE(engine->errorString, QString());

    bool result = true;
    StyleCache::StyleData *rule;
    Selector path, expected;

    path = Selector(".baseA");
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (rule->selector() == path);
    QCOMPARE(result, true);

    path = Selector("testA baseA");
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (rule->selector() == path);
    QCOMPARE(result, true);

    path = Selector("testA>baseA");
    expected = Selector("testA baseA");
    rule = engine->styleRuleForPath(path);
    // should pass, but should be ".testA .baseA"
    result = (rule != 0) && (rule->selector() == expected);
    QCOMPARE(result, true);

    path = Selector("testB>baseA");
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (rule->selector() == path);
    QCOMPARE(result, true);

    path = Selector("testB baseA");
    rule = engine->styleRuleForPath(path);
    QVERIFY2(rule != 0, "Rule not found.");
    // should fail
    result = (rule != 0) && (rule->selector() != path);
    QCOMPARE(result, true);

    path = Selector("testC testB baseA");
    rule = engine->styleRuleForPath(path);
    QVERIFY2(rule != 0, "Rule not found.");
    result = (rule != 0) && (rule->selector() == path);
    QCOMPARE(result, true);

    path = Selector("testC>testB baseA");
    rule = engine->styleRuleForPath(path);
    QVERIFY2(rule != 0, "Rule not found.");
    result = (rule != 0) && (rule->selector() == path);
    QCOMPARE(result, true);

    path = Selector("testC>testB baseA");
    rule = engine->styleRuleForPath(Selector("testC>testB>baseA"));
    QVERIFY2(rule != 0, "Rule not found.");
    // FIXME: this is a bug, need to have a fix in a separate MR
    result = (rule != 0) && (rule->selector() != path);
    QCOMPARE(result, true);
}

void tst_ThemeEnginePrivate::testCase_parseSelector()
{
    engine->errorString = QString();

    // build selector path
    QList<Selector> selectors = engine->parseSelector(".testB .baseA");
    Selector expected("testB baseA");
    // should match
    bool result = (selectors.count() == 1) && (selectors[0] == expected);
    QCOMPARE(result, true);

    selectors = engine->parseSelector(".testB > .baseA");
    // should not match
    result = (selectors.count() == 1) && !(selectors[0] == expected);
    QCOMPARE(result, true);

    expected = Selector("root#id>testB baseB");
    selectors = engine->parseSelector(".root#id .testB > .baseA");
    // should not match!
    result = (selectors.count() == 1) && !(selectors[0] == expected);
    QCOMPARE(result, true);

    selectors = engine->parseSelector(".root#id>.testB .baseB");
    // should match
    result = (selectors.count() == 1) && (selectors[0] == expected);
    QCOMPARE(result, true);

    selectors = engine->parseSelector(".root#id > .testB .baseB, .oneNode.bing .baseC");
    Selector expected2(".oneNode.bing .baseC");
    QCOMPARE(selectors.count(), 2);
    QCOMPARE(selectors[0], expected);
    QCOMPARE(selectors[1], expected2);
}

void tst_ThemeEnginePrivate::testCase_selectorToString()
{
    engine->errorString = QString();
    Selector selector;
    QString expected;

    selector = Selector(".classA .classB");
    expected = ".classa .classb";
    QCOMPARE(selector.toString(), expected);

    selector = Selector(".classA > .classB");
    expected = ".classa>.classb";
    QCOMPARE(selector.toString(), expected);

    selector = Selector(".classA#id .classB");
    expected = ".classa#id .classb";
    QCOMPARE(selector.toString(), expected);

    selector = Selector("   .classA   >   .classB#id  ");
    expected = ".classa>.classb#id";
    QCOMPARE(selector.toString(), expected);

    selector = Selector(".classA.attribute>.classB#id");
    expected = ".classa.attribute>.classb#id";
    QCOMPARE(selector.toString(), expected);
}

void tst_ThemeEnginePrivate::testCase_inheritance()
{
    engine->errorString = QString();
    engine->loadTheme(QUrl::fromLocalFile("../../resources/inheritance.qmltheme"));
    QVERIFY(engine->errorString.isEmpty());
    // using root selector node (first node of derived list)

    QVERIFY(!test_styleProperties(".derivate", "pDerivate:pDerivate,pBaseA:pBaseA", true));
    QVERIFY(!test_styleProperties(".derivate2", "pDerivate:pDerivate,pBaseA:derivate2", true));
    QVERIFY(!test_styleProperties(".multiple", "pBaseA:pBaseA,pBaseB:pBaseB", true));
    QVERIFY(!test_styleProperties(".multiple2", "pBaseA:pBaseA,pDerivate:pDerivate,pMultiple2:multiple2", true));
    QVERIFY(!test_styleProperties(".multiple3", "pDerivate:multiple3,pBaseA:derivate2,pBaseB:pBaseB", true));
    QVERIFY(!test_styleProperties(".restore", "pBaseA:pBaseA,pDerivate:pDerivate", true));

    // use complete selector (multiple classes)
    QVERIFY(test_styleProperties(".derivate.basea", "pDerivate:pDerivate,pBaseA:pBaseA", false));
    QVERIFY(!test_styleProperties(".derivate2.derivate", "pDerivate:pDerivate,pBaseA:derivate2", true));
    QVERIFY(test_styleProperties(".multiple.basea.baseb", "pBaseA:pBaseA,pBaseB:pBaseB", false));
    QVERIFY(!test_styleProperties(".multiple2.baseA.derivate", "pBaseA:pBaseA,pDerivate:pDerivate,pMultiple2:multiple2", true));
    QVERIFY(!test_styleProperties(".multiple3.derivate2.baseB", "pDerivate:multiple3,pBaseA:derivate2,pBaseB:pBaseB",true));
    QVERIFY(!test_styleProperties(".restore.derivate2.baseA", "pBaseA:pBaseA,pDerivate:pDerivate", true));
}

// tests the properties of a style class, propertyList is a pair of property:value
// returns true if test passes, false if not
bool tst_ThemeEnginePrivate::test_styleProperties(const QString &styleClass, const QString &propertyList, bool xfail)
{
    Selector selector = engine->parseSelector(styleClass)[0];
    StyleCache::StyleData *rule = engine->styleRuleForPath(selector);
    if ((xfail && rule) || !xfail)
        QVERIFY2_RET(rule, "Failure");
    if (rule) {
        // create style from the rule so we can check the validity of the URLs
        QObject *style = rule->style ? rule->style->create(quickEngine->rootContext()) : 0;
        if ((xfail && style) || !xfail)
            QVERIFY2_RET(style, "FAILURE");

        if (style) {
            Q_FOREACH(const QString &propertyPair, propertyList.split(',')) {
                QStringList pair = propertyPair.split(':');
                QString data = style->property(pair[0].toLatin1()).toString();
                if ((xfail && !data.isEmpty()) || !xfail)
                    QCOMPARE_RET(data, pair[1]);
            }
            style->deleteLater();
        }
    }
    return !xfail;
}


QTEST_MAIN(tst_ThemeEnginePrivate)

#include "tst_theme_engine_privatetest.moc"
