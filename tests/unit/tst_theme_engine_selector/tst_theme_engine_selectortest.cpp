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
#include "selector_p.h"

class tst_ThemeEngineSelector : public QObject
{
    Q_OBJECT
    
public:
    tst_ThemeEngineSelector() {}
    
private Q_SLOTS:
    void testCase_SelectorNodeConstructEmpty()
    {
        SelectorNode node;
        QVERIFY(node.isEmpty());
        QVERIFY(!node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankNull);
    }

    void testCase_SelectorNodeConstructClass()
    {
        SelectorNode node(".class");
        QVERIFY(!node.isEmpty());
        QVERIFY(!node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankNull);
        QCOMPARE(node.getClass(), QString("class"));
        QCOMPARE(node.id(), QString());
    }

    void testCase_SelectorNodeConstructId()
    {
        SelectorNode node("#id");
        QVERIFY(!node.isEmpty());
        QVERIFY(!node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankId);
        QCOMPARE(node.getClass(), QString());
        QCOMPARE(node.id(), QString("id"));
    }

    void testCase_SelectorNodeConstructClassAndId()
    {
        SelectorNode node(".class#id");
        QVERIFY(!node.isEmpty());
        QVERIFY(!node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankId);
        QCOMPARE(node.getClass(), QString("class"));
        QCOMPARE(node.id(), QString("id"));
    }

    void testCase_SelectorNodeConstructChildClass()
    {
        SelectorNode node(">.class");
        QVERIFY(!node.isEmpty());
        QVERIFY(node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankChild);
        QCOMPARE(node.getClass(), QString("class"));
        QCOMPARE(node.id(), QString());
    }

    void testCase_SelectorNodeConstructChildId()
    {
        SelectorNode node(">#id");
        QVERIFY(!node.isEmpty());
        QVERIFY(node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankFull);
        QCOMPARE(node.getClass(), QString());
        QCOMPARE(node.id(), QString("id"));
    }

    void testCase_SelectorNodeConstructChildClassAndId()
    {
        SelectorNode node(">.class#id");
        QVERIFY(!node.isEmpty());
        QVERIFY(node.isStrictRelationed());
        QVERIFY(node.rank() == SelectorNode::RankFull);
        QCOMPARE(node.getClass(), QString("class"));
        QCOMPARE(node.id(), QString("id"));
    }

    void testCase_SelectorNodeDetailedConstruct()
    {
        SelectorNode node("type", "class", "id", SelectorNode::Descendant);
        QVERIFY(!node.isEmpty());
        QVERIFY(!node.isStrictRelationed());
        QVERIFY(node.rank() == 1);
        QCOMPARE(node.getClass(), QString("class"));
        QCOMPARE(node.id(), QString("id"));
    }

    void testCase_SelectorNodeToString()
    {
        SelectorNode node(">.class.derivate#id");

        QCOMPARE(node.toString(), QString(">.class.derivate#id"));
        QCOMPARE(node.toString(SelectorNode::NoDerivates), QString(">.class#id"));
        QCOMPARE(node.toString(SelectorNode::NoStyleId), QString(">.class.derivate"));
        QCOMPARE(node.toString(SelectorNode::NoRelation), QString(".class.derivate#id"));
        QCOMPARE(node.toString(SelectorNode::NoRelation | SelectorNode::NoDerivates),
                 QString(".class#id"));
        QCOMPARE(node.toString(SelectorNode::IgnoreAll), QString(".class"));
    }

    void testCase_SelectorNodeOperator()
    {
        SelectorNode node1(">.class.derivate#id");
        SelectorNode node2(">.class.derivate#id");

        QVERIFY(node1 == node2);

        node1 = SelectorNode(node1, SelectorNode::NoDerivates);
        node2 = SelectorNode(node2, SelectorNode::NoDerivates);
        QVERIFY(node1 == node2);

        node1 = SelectorNode(node1, SelectorNode::NoRelation);
        QVERIFY(!(node1 == node2));
    }

    void testCase_SelectorNodeCopyConstructors()
    {
        SelectorNode node(">.class.derivate#id");

        SelectorNode copy(node);
        QVERIFY(node == copy);

        copy = SelectorNode(node, SelectorNode::NoDerivates);
        QCOMPARE(copy.toString(), QString(">.class#id"));

        copy = SelectorNode(node, SelectorNode::NoRelation);
        QCOMPARE(copy.toString(), QString(".class.derivate#id"));

        copy = SelectorNode(node, SelectorNode::NoStyleId);
        QCOMPARE(copy.toString(), QString(">.class.derivate"));

        copy = SelectorNode(node, SelectorNode::IgnoreAll);
        QCOMPARE(copy.toString(), QString(".class"));
    }

    void testCase_SelectorNodeMultipleClasses()
    {
        SelectorNode node(".classA.classB.classC");
        QCOMPARE(node.getClass(), QString("classa"));
        QCOMPARE(node.derivates(), QString(".classb.classc"));

        node = SelectorNode(">.classA.classB.classC#id");
        QCOMPARE(node.getClass(), QString("classa"));
        QCOMPARE(node.derivates(), QString(".classb.classc"));

        node = SelectorNode(">.classA.classB#id");
        QCOMPARE(node.getClass(), QString("classa"));
        QCOMPARE(node.derivates(), QString(".classb"));

        node = SelectorNode(">.classA#id");
        QCOMPARE(node.getClass(), QString("classa"));
        QCOMPARE(node.derivates(), QString());
    }

    /*
     * Selector
     */
    void testCase_SelectorConstructorDefault()
    {
        Selector selector;
        QVERIFY(selector.size() == 0);
        QCOMPARE(selector.toString(), QString());
        QVERIFY(selector.rank() == 0);
    }

    void testCase_SelectorConstructorFromString()
    {
        Selector selector(".classA");
        QVERIFY(selector.size() == 1);
        QCOMPARE(selector.toString(), QString(".classa"));
        QVERIFY(selector.rank() == 0);

        selector = Selector(".classA .classB");
        QVERIFY(selector.size() == 2);
        QCOMPARE(selector.toString(), QString(".classa .classb"));
        QVERIFY(selector.rank() == 0);

        selector = Selector(".classA#nameA .classB");
        QVERIFY(selector.size() == 2);
        QCOMPARE(selector.toString(), QString(".classa#namea .classb"));
        QVERIFY(selector.rank() == 1);

        selector = Selector(".classA#nameA>.classB");
        QVERIFY(selector.size() == 2);
        QCOMPARE(selector.toString(), QString(".classa#namea>.classb"));
        QVERIFY(selector.rank() == 0x9);

        selector = Selector(".classA#nameA>.classB#nameB");
        QVERIFY(selector.size() == 2);
        QCOMPARE(selector.toString(), QString(".classa#namea>.classb#nameb"));
        qDebug() << selector.rank();
        QVERIFY(selector.rank() == 0xD);
    }

    void testCase_SelectorCopyConstructor()
    {
        Selector selector(".classA");
        QVERIFY(selector.size() == 1);
        QCOMPARE(selector.toString(), QString(".classa"));
        QVERIFY(selector.rank() == 0);

        Selector copy(selector);
        QCOMPARE(copy, selector);
    }
};

QTEST_MAIN(tst_ThemeEngineSelector)

#include "tst_theme_engine_selectortest.moc"
