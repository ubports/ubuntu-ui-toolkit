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

class tst_ThemeEngineStyleCache : public QObject
{
    Q_OBJECT
    
public:
    tst_ThemeEngineStyleCache() {}
private:
    StyleCache cache;

private Q_SLOTS:

    void testCase_addDelegate()
    {
        QObjectCleanupHandler cleanup;
        QQmlComponent *delegate = new QQmlComponent;
        cleanup.add(delegate);
        QVERIFY(cache.addDelegate("test", delegate));
        cache.clear();
        QVERIFY(cleanup.isEmpty());
    }

    void testCase_addStyleRule()
    {
        Selector selector;
        StyleCache::StyleData *rule;

        selector = Selector(".baseA");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseB");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA .baseB");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA>.baseB");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA .baseB .baseC");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA .baseB>.baseC");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA>.baseB .baseC");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA .baseB .baseC .baseD");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);

        selector = Selector(".baseA .baseB>.baseC .baseD");
        cache.addStyleRule(selector, new QQmlComponent, new QQmlComponent);
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), selector);
    }

    void testCase_match()
    {
        Selector selector, expected;
        StyleCache::StyleData *rule;

        selector = Selector(".baseA>.baseB>.baseC");
        expected = Selector(".baseA .baseB>.baseC");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA>.baseB>.baseC#id");
        expected = Selector(".baseA .baseB>.baseC");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA>.baseB#id>.baseC");
        rule = cache.match(selector);
        QVERIFY(!rule);

        selector = Selector(".baseA>.baseB>.baseC>.baseD");
        expected = Selector(".baseA .baseB>.baseC .baseD");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA>.baseB .baseC>.baseD");
        expected = Selector(".baseA .baseB .baseC .baseD");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA>.baseB .baseC>.baseD#id");
        expected = Selector(".baseA .baseB .baseC .baseD");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA>.baseB .baseC .baseD");
        expected = Selector(".baseA .baseB .baseC .baseD");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA .baseB .baseC .baseD#id");
        expected = Selector(".baseA .baseB .baseC .baseD");
        rule = cache.match(selector);
        QVERIFY(rule);
        QCOMPARE(rule->selector(), expected);

        selector = Selector(".baseA .baseB>.baseC#id .baseD");
        rule = cache.match(selector);
        QVERIFY(!rule);

        selector = Selector(".baseA .baseB .baseC#id .baseD");
        rule = cache.match(selector);
        QVERIFY(!rule);
    }

    void testCase_styleNodeLeak()
    {
        StyleCache cache;
        QObjectCleanupHandler cleanup;

        QQmlComponent *style = new QQmlComponent();
        QQmlComponent *delegate = new QQmlComponent();
        cleanup.add(style);
        cleanup.add(delegate);
        cache.addStyleRule(Selector(".nodeb"), style, delegate);

        style = new QQmlComponent();
        delegate = new QQmlComponent();
        cleanup.add(style);
        cleanup.add(delegate);
        cache.addStyleRule(Selector(".nodea .nodeb"), style, delegate);

        style = new QQmlComponent();
        delegate = new QQmlComponent();
        cleanup.add(style);
        cleanup.add(delegate);
        cache.addStyleRule(Selector(".nodea .nodeb"), style, delegate);

        cache.clear();
        QVERIFY(cleanup.isEmpty());
    }

};

QTEST_MAIN(tst_ThemeEngineStyleCache)

#include "tst_theme_engine_stylecachetest.moc"
