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
#include "ucstyle.h"

class tst_ThemeEngineStyle : public QObject
{
    Q_OBJECT
    
public:
    tst_ThemeEngineStyle() {}

private:
    QQuickView *quickView;
    QQmlEngine *quickEngine;
    QQmlContext *context;

    QQuickItem *boundItem;
    StyledPropertyMap watchList;

    UCStyle *testingStyle;

    QQuickItem *testItem()
    {
        ThemeEngine::initializeEngine(quickEngine);
        ThemeEngine::instance()->resetError();
        quickView->setSource(QUrl::fromLocalFile("TestDocument.qml"));
        QCoreApplication::processEvents();

        QObject *root = quickView->rootObject();
        if (!root)
            return 0;

        QList<QQuickItem*> items = root->findChildren<QQuickItem*>();
        Q_FOREACH(QQuickItem *item, items) {
            // if a style has Item-derived properties (Animations, etc), those will be listed here too
            // therefore skip those
            QObject *obj = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
            if (obj)
                return item;
        }
        return 0;
    }

protected Q_SLOTS:
    void watchBoundItemProperty()
    {
        if (!sender() || !testingStyle)
            return;
        QString property = QString(sender()->metaObject()->method(senderSignalIndex()).name()).remove("Changed");
        QVERIFY(testingStyle->isUpdating(property));
    }

private Q_SLOTS:

    void initTestCase()
    {
        testingStyle = 0;

        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        quickView = new QQuickView(0);
        quickEngine = quickView->engine();

        quickView->setGeometry(0,0, 240, 320);
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports << QDir(modules).absolutePath();
        quickEngine->setImportPathList(imports);

        bool result = (ThemeEngine::initializeEngine(quickEngine) != 0);
        // theme loading might fail, however don't care about it
        QVERIFY(result);
        ThemeEngine::instance()->resetError();

        boundItem = testItem();
        QVERIFY(boundItem);

        for (int i = 0; i < boundItem->metaObject()->propertyCount(); i++) {
            const QMetaProperty property = boundItem->metaObject()->property(i);
            if (UCStyle::omitProperty(property.name()))
                continue;
            if (!property.hasNotifySignal())
                continue;
            watchList.mark(i, StyledPropertyMap::Enabled);
            const QMetaMethod slot = metaObject()->method(metaObject()->indexOfSlot("watchBoundItemProperty()"));
            QObject::connect(boundItem, property.notifySignal(), this, slot);
        }

        // create context for style and delegate
        context = new QQmlContext(QQmlEngine::contextForObject(boundItem));

        // publish boundItem as context property
        context->setContextProperty("item", boundItem);
    }

    void cleanupTestCase()
    {
        delete context;
        delete quickView;
    }

    void testCase_bindItem()
    {
        StyleCache::StyleData *rule = ThemeEnginePrivate::styleRuleForPath(Selector("button"));
        QVERIFY(rule);
        QVERIFY(rule->style);
        QVERIFY(rule->delegate);

        QObject *obj = rule->style->create(context);
        QVERIFY(obj);

        UCStyle *style = qobject_cast<UCStyle*>(obj);
        QVERIFY(style);

        obj = rule->delegate->create(context);
        QVERIFY(obj);
        QQuickItem *delegate = qobject_cast<QQuickItem*>(obj);
        QVERIFY(delegate);

        QCOMPARE(style->bindItem(0, watchList), -1);
        QCOMPARE(style->bindItem(boundItem, watchList), -1);
        QCOMPARE(style->bindItem(delegate, watchList), -1);
        style->setParent(boundItem);
        QCOMPARE(style->bindItem(boundItem, watchList), 1);
        QCOMPARE(style->bindItem(delegate, watchList), -1);
        delegate->setParentItem(boundItem);
        QCOMPARE(style->bindItem(delegate, watchList), 2);

        delete delegate;
        delete style;
    }

    void testCase_unbindItem()
    {
        StyleCache::StyleData *rule = ThemeEnginePrivate::styleRuleForPath(Selector("button"));
        QVERIFY(rule);
        QVERIFY(rule->style);
        QVERIFY(rule->delegate);

        QObject *obj = rule->style->create(context);
        QVERIFY(obj);
        UCStyle *style = qobject_cast<UCStyle*>(obj);
        QVERIFY(style);

        style->setParent(boundItem);
        QCOMPARE(style->bindItem(boundItem, watchList), 1);

        obj = rule->delegate->create(context);
        QVERIFY(obj);
        QQuickItem *delegate = qobject_cast<QQuickItem*>(obj);
        QVERIFY(delegate);

        // unbind
        QVERIFY(style->unbindItem(boundItem));
        QVERIFY(!style->unbindItem(boundItem));
        QVERIFY(!style->unbindItem(delegate));
        QVERIFY(!style->unbindItem(0));

        delete delegate;
        delete style;
    }

    void testCase_unbindProperty()
    {
        StyleCache::StyleData *rule = ThemeEnginePrivate::styleRuleForPath(Selector("button"));
        QVERIFY(rule);
        QVERIFY(rule->style);
        QVERIFY(rule->delegate);

        QObject *obj = rule->style->create();
        QVERIFY(obj);
        UCStyle *style = qobject_cast<UCStyle*>(obj);
        QVERIFY(style);
        style->setParent(boundItem);

        obj = rule->delegate->create(context);
        QVERIFY(obj);
        QQuickItem *delegate = qobject_cast<QQuickItem*>(obj);
        QVERIFY(delegate);
        delegate->setParent(boundItem);
        delegate->setParentItem(boundItem);

        QCOMPARE(style->bindItem(boundItem, watchList), 1);
        QCOMPARE(style->bindItem(delegate, watchList), 2);

        QVERIFY(style->unbindProperty("color"));
        QVERIFY(!style->unbindProperty("width"));
        QVERIFY(!style->unbindProperty("height"));
        QVERIFY(style->unbindProperty("borderSource"));
        QVERIFY(style->unbindProperty("borderPressed"));
        QVERIFY(!style->unbindProperty("color"));

        delete delegate;
        delete style;
    }

};

QTEST_MAIN(tst_ThemeEngineStyle)

#include "tst_theme_engine_styletest.moc"
