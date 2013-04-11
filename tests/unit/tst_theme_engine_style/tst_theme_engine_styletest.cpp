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

    UCStyle *testingStyle;

    QQuickItem *testItem(const QString &document, StyledPropertyMap &watchList, const QUrl &theme = QUrl())
    {
        // delete previous root
        QObject *root = quickView->rootObject();
        if (root) delete root;

        ThemeEngine::initializeEngine(quickEngine);
        if (theme.isValid()) {
            ThemeEngine::instance()->loadTheme(theme);
            if (!ThemeEngine::instance()->error().isEmpty()) {
                QWARN("Theme loading failed");
                return 0;
            }
        } else
            ThemeEngine::instance()->resetError();
        quickView->setSource(QUrl::fromLocalFile(document));
        QCoreApplication::processEvents();

        root = quickView->rootObject();
        if (!root)
            return 0;

        QList<QQuickItem*> items = root->findChildren<QQuickItem*>();
        Q_FOREACH(QQuickItem *item, items) {
            // if an item has Item-derived properties (Animations, etc), those will be listed here too
            // therefore skip those
            QObject *obj = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
            if (obj) {

                for (int i = 0; i < item->metaObject()->propertyCount(); i++) {
                    const QMetaProperty property = item->metaObject()->property(i);
                    if (UCStyle::omitProperty(property.name()))
                        continue;
                    if (!property.hasNotifySignal())
                        continue;
                    watchList.mark(i, StyledPropertyMap::Enabled);
                    const QMetaMethod slot = metaObject()->method(metaObject()->indexOfSlot("watchBoundItemProperty()"));
                    QObject::connect(item, property.notifySignal(), this, slot);
                }

                // publish boundItem as context property
                context->setContextProperty("item", item);

                return item;
            }
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
        QVERIFY(result);
        // check if theme gets loaded
        QCOMPARE(ThemeEngine::instance()->error(), QString(""));

        // create context for style and delegate
        context = new QQmlContext(quickEngine->rootContext());
    }

    void cleanupTestCase()
    {
        delete quickView;
    }

    void testCase_omitProperty()
    {
        // to be omitted
        QString omitList(
                "activeFocus,anchors,antialiasing,baseline,baselineOffset,"
                "bottom,children,childrenRect,clip,data,focus,"
                "horizontalCenter,implicitHeight,implicitWidth,layer,left,objectName,parent,"
                "resources,right,rotation,scale,smooth,state,states,top,transform,transformOrigin,"
                "transitions,verticalCenter,visibleChildren,x,y,z");
        QString allowedList(
                    "enabled,width,height,opacity,visible,source,item"
                    );

        Q_FOREACH(const QString &property, omitList.split(',')) {
            QVERIFY2(UCStyle::omitProperty(property.toLocal8Bit()), property.toLocal8Bit());
        }

        Q_FOREACH(const QString &property, allowedList.split(',')) {
            QVERIFY2(!UCStyle::omitProperty(property.toLocal8Bit()), property.toLocal8Bit());
        }
    }

    void testCase_bindItem()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("TestDocument.qml", watchList);

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

        QCOMPARE(style->bindItem(0, watchList, false), -1);
        QCOMPARE(style->bindItem(boundItem, watchList, true), 1);
        QCOMPARE(style->bindItem(delegate, watchList, false), 2);

        delete delegate;
        delete style;
    }

    void testCase_unbindItem()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("TestDocument.qml", watchList);

        StyleCache::StyleData *rule = ThemeEnginePrivate::styleRuleForPath(Selector("button"));
        QVERIFY(rule);
        QVERIFY(rule->style);
        QVERIFY(rule->delegate);

        QObject *obj = rule->style->create(context);
        QVERIFY(obj);
        UCStyle *style = qobject_cast<UCStyle*>(obj);
        QVERIFY(style);

        QCOMPARE(style->bindItem(boundItem, watchList, true), 1);

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
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("TestDocument.qml", watchList);

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

        QCOMPARE(style->bindItem(boundItem, watchList, true), 1);
        QCOMPARE(style->bindItem(delegate, watchList, false), 2);

        QVERIFY(style->unbindProperty("color"));
        QVERIFY(!style->unbindProperty("width"));
        QVERIFY(!style->unbindProperty("height"));
        QVERIFY(style->unbindProperty("borderSource"));
        QVERIFY(style->unbindProperty("borderPressed"));
        QVERIFY(!style->unbindProperty("color"));

        delete delegate;
        delete style;
    }

    void testCase_binding()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("BindingTest.qml", watchList);
        QVERIFY(boundItem);

        ItemStyleAttached *itemStyle = qobject_cast<ItemStyleAttached*>
                (qmlAttachedPropertiesObject<ItemStyleAttached>(boundItem, false));
        QVERIFY(itemStyle);

        // verify style class, should be button
        QString sclass = itemStyle->property("class").toString();
        QCOMPARE(sclass, QString("button"));

        // color property is bound to otherColor property; styling should update this
        // properties as well, only user bindings (bindings made upon component use)
        // should not be broken
        QColor color(boundItem->property("color").toString());
        QCOMPARE(color, QColor("#cccccc"));
        // modify binding property value
        boundItem->setProperty("otherColor", "blue");
        color = QColor(boundItem->property("color").toString());
        QCOMPARE(color, QColor("#cccccc"));
    }

    void testCase_fontThemeNoOverride()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("FontThemeNoOverride.qml", watchList, QUrl::fromLocalFile("FontThemeNoOverride.qmltheme"));
        QFont font = boundItem->property("font").value<QFont>();
        QVERIFY(font.weight() == QFont::Bold);
    }

    void testCase_fontThemeOverrideDefaultValue()
    {
        QEXPECT_FAIL("", "Default values for 'font' subproperties cannot be overriden by the theme.", Continue);
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("FontThemeOverrideDefaultValue.qml", watchList, QUrl::fromLocalFile("FontThemeOverrideDefaultValue.qmltheme"));
        QFont font = boundItem->property("font").value<QFont>();
        QVERIFY(font.weight() == QFont::Bold);
    }

    void testCase_fontThemeDoNotOverrideUserValue()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("FontThemeDoNotOverrideUserValue.qml", watchList, QUrl::fromLocalFile("FontThemeDoNotOverrideUserValue.qmltheme"));
        QFont font = boundItem->property("font").value<QFont>();
        QVERIFY(font.weight() == QFont::Light);
    }

    void testCase_fontThemeNoOverrideWithBinding()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("FontThemeNoOverrideWithBinding.qml", watchList, QUrl::fromLocalFile("FontThemeNoOverrideWithBinding.qmltheme"));
        QFont font = boundItem->property("font").value<QFont>();
        QVERIFY(font.weight() == QFont::Bold);
        int pixelSizeSource = boundItem->property("pixelSizeSource").value<int>();
        QCOMPARE(font.pixelSize(), pixelSizeSource + 2);
    }

    void testCase_fontThemeNoOverrideUserBinding()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("FontThemeNoOverrideUserBinding.qml", watchList, QUrl::fromLocalFile("FontThemeNoOverrideUserBinding.qmltheme"));
        QFont font = boundItem->property("font").value<QFont>();
        QVERIFY(font.weight() == QFont::Bold);
        int pixelSizeSource = boundItem->property("pixelSizeSource").value<int>();
        QCOMPARE(font.pixelSize(), pixelSizeSource + 2);
    }

    void testCase_fontThemeWithOtherDefaultValue()
    {
        StyledPropertyMap watchList;
        QQuickItem *boundItem = testItem("FontThemeWithOtherDefaultValue.qml", watchList, QUrl::fromLocalFile("FontThemeWithOtherDefaultValue.qmltheme"));
        QFont font = boundItem->property("font").value<QFont>();
        QCOMPARE(font.underline(), true);
        QVERIFY(font.weight() == QFont::Bold);
    }
};

QTEST_MAIN(tst_ThemeEngineStyle)

#include "tst_theme_engine_styletest.moc"
