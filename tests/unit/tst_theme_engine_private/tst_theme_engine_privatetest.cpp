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
#include "suffixtree_p.h"

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
    StyleTreeNode *rule = engine->styleRuleForPath(selector);
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
    StyleTreeNode *rule;
    Selector path, expected;

    path << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (rule->path() == path);
    QCOMPARE(result, true);

    path.clear();
    path << SelectorNode("testA", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (rule->path() == path);
    QCOMPARE(result, true);

    path.clear();
    path << SelectorNode("testA", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Child);
    expected << SelectorNode("testA", "", SelectorNode::Descendant);
    expected << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should pass, but should be ".testA .baseA"
    result = (rule != 0) && (rule->path() == expected);
    QCOMPARE(result, true);

    path.clear();
    path << SelectorNode("testB", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Child);
    rule = engine->styleRuleForPath(path);
    // should pass
    result = (rule != 0) && (rule->path() == path);
    QCOMPARE(result, true);

    path.clear();
    path << SelectorNode("testB", "", SelectorNode::Descendant);
    path << SelectorNode("baseA", "", SelectorNode::Descendant);
    rule = engine->styleRuleForPath(path);
    // should fail
    result = (rule != 0) && !(rule->path() == path);
    QCOMPARE(result, true);
}

void tst_ThemeEnginePrivate::testCase_parseSelector()
{
    engine->errorString = QString();

    // build selector path
    QList<Selector> selectors = engine->parseSelector(".testB .baseA");
    Selector expected, expected2;
    expected << SelectorNode("testB", "", SelectorNode::Descendant);
    expected << SelectorNode("baseA", "", SelectorNode::Descendant);
    // should match
    bool result = (selectors.count() == 1) && (selectors[0] == expected);
    QCOMPARE(result, true);

    selectors = engine->parseSelector(".testB > .baseA");
    // should not match
    result = (selectors.count() == 1) && !(selectors[0] == expected);
    QCOMPARE(result, true);

    expected.clear();
    expected << SelectorNode("root", "id", SelectorNode::Descendant);
    expected << SelectorNode("testB", "", SelectorNode::Child);
    expected << SelectorNode("baseB", "", SelectorNode::Descendant);
    selectors = engine->parseSelector(".root#id .testB > .baseA");
    // should not match!
    result = (selectors.count() == 1) && !(selectors[0] == expected);
    QCOMPARE(result, true);

    selectors = engine->parseSelector(".root#id > .testB .baseB");
    // should match
    result = (selectors.count() == 1) && (selectors[0] == expected);
    QCOMPARE(result, true);

    selectors = engine->parseSelector(".root#id > .testB .baseB, .oneNode.bing .baseC");
    expected2 << SelectorNode("oneNode.bing", "", SelectorNode::Descendant);
    expected2 << SelectorNode("baseC", "", SelectorNode::Descendant);
    result = (selectors.count() == 2) &&
            (selectors[0] == expected) &&
            (selectors[1] == expected2);
    QCOMPARE(result, true);
}

void tst_ThemeEnginePrivate::testCase_selectorToString()
{
    engine->errorString = QString();
    bool result = true;
    Selector selector;
    QString expected;

    selector.clear();
    selector << SelectorNode("classA", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "", SelectorNode::Descendant);
    expected = ".classa .classb";
    result = engine->selectorToString(selector) == expected;
    QCOMPARE(result, true);

    selector.clear();
    selector << SelectorNode("classA", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "", SelectorNode::Child);
    expected = ".classa > .classb";
    result = engine->selectorToString(selector) == expected;
    QCOMPARE(result, true);

    selector.clear();
    selector << SelectorNode("classA", "id", SelectorNode::Descendant);
    selector << SelectorNode("classB", "", SelectorNode::Descendant);
    expected = ".classa#id .classb";
    result = engine->selectorToString(selector) == expected;
    QCOMPARE(result, true);

    selector.clear();
    selector << SelectorNode("classA", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "id", SelectorNode::Child);
    expected = ".classa > .classb#id";
    result = engine->selectorToString(selector) == expected;
    QCOMPARE(result, true);

    selector.clear();
    selector << SelectorNode("classA.attribute", "", SelectorNode::Descendant);
    selector << SelectorNode("classB", "id", SelectorNode::Child);
    expected = ".classa.attribute > .classb#id";
    result = engine->selectorToString(selector) == expected;
    QCOMPARE(result, true);
}

QTEST_MAIN(tst_ThemeEnginePrivate)

#include "tst_theme_engine_privatetest.moc"
