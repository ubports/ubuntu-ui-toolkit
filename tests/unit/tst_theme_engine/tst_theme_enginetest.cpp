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

#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

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
    void testCase_reparenting();
    void testCase_blockDeclaration();
    void testCase_selectorDelegates();

private:
    QQuickView *quickView;
    QQmlEngine *quickEngine;
};

tst_ThemeEngine::tst_ThemeEngine():
    quickView(0),
    quickEngine(0)
{
}

void tst_ThemeEngine::initTestCase()
{
    QString modules("../../../modules");
    QVERIFY(QDir(modules).exists());

    quickView = new QQuickView(0);
    quickEngine = quickView->engine();

    quickView->setGeometry(0,0, 240, 320);
    //add modules folder so we have access to the plugin from QML
    QStringList imports = quickEngine->importPathList();
    imports << QDir(modules).absolutePath();
    quickEngine->setImportPathList(imports);
}

void tst_ThemeEngine::cleanupTestCase()
{
    delete quickView;
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

void tst_ThemeEngine::testCase_reparenting()
{
    ThemeEngine::initializeEngine(quickEngine);
    ThemeEngine::instance()->resetError();
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/test.qmltheme"));
    quickView->setSource(QUrl::fromLocalFile("ReparentingTest.qml"));
    QCoreApplication::processEvents();

    QObject *root = quickView->rootObject();
    QVERIFY2(root, "FAILURE");

    QCOMPARE(root->property("themeError").toString(), QString());

    QList<QQuickItem*> items = root->findChildren<QQuickItem*>();
    QVERIFY(items.count());

    Q_FOREACH(QQuickItem *item, items) {
        // if a style has Item-derived properties (Animations, etc), those will be listed here too
        // therefore skip those
        QObject *obj = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
        if (!obj)
            continue;

        ItemStyleAttached *attached = qobject_cast<ItemStyleAttached*>(obj);
        QVERIFY(attached);

        QObject *style = qvariant_cast<QObject*>(attached->property("style"));
        QVERIFY(style);

        if (attached->styleClass() == "testa") {
            QCOMPARE(attached->path(), QString(".testa"));
            QCOMPARE(style->property("prop_testA_A").toString(), QString("testA.prop_testA_A"));
        } else if (attached->styleClass() == "basea") {
            QCOMPARE(attached->path(), QString(".testa .basea"));
            QCOMPARE(style->property("prop_baseA_A").toString(), QString("overload:: test.qmltheme/.baseA"));
            QCOMPARE(style->property("prop_baseA_B").toString(), QString("prop_baseA_B"));
            QCOMPARE(style->property("prop_baseA_C").toString(), QString("overload-> test.qmltheme/.testA .baseA"));
            QCOMPARE(style->property("test_base_A_A").toString(), QString("new:: test.qmltheme/.baseA"));
        } else {
            QVERIFY(0);
        }
    }
}

void tst_ThemeEngine::testCase_blockDeclaration()
{
    ThemeEngine::initializeEngine(quickEngine);
    ThemeEngine::instance()->resetError();
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/block.qmltheme"));
    quickView->setSource(QUrl::fromLocalFile("BlockPropertyTest.qml"));

    QObject *root = quickView->rootObject();
    QVERIFY2(root, "FAILURE");

    QObject *obj = qmlAttachedPropertiesObject<ItemStyleAttached>(root, false);
    QVERIFY(obj);

    ItemStyleAttached *attached = qobject_cast<ItemStyleAttached*>(obj);
    QVERIFY(attached);

    QObject *style = qvariant_cast<QObject*>(attached->property("style"));
    QVERIFY(style);

    QObject *anim = qvariant_cast<QObject*>(style->property(""));
}

void tst_ThemeEngine::testCase_selectorDelegates()
{
    ThemeEngine::initializeEngine(quickEngine);
    ThemeEngine::instance()->resetError();
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/test.qmltheme"));
    quickView->setSource(QUrl::fromLocalFile("SelectorTest.qml"));
    QCoreApplication::processEvents();

    QObject *root = quickView->rootObject();
    QVERIFY2(root, "FAILURE");

    QCOMPARE(root->property("themeError").toString(), QString());

    QList<QQuickItem*> items = root->findChildren<QQuickItem*>();
    QVERIFY(items.count());

    Q_FOREACH(QQuickItem *item, items) {
        // if a style has Item-derived properties (Animations, etc), those will be listed here too
        // therefore skip those
        QObject *obj = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
        if (!obj)
            continue;

        ItemStyleAttached *attached = qobject_cast<ItemStyleAttached*>(obj);
        QVERIFY(attached);

        QQuickItem *delegate = qvariant_cast<QQuickItem*>(attached->property("delegate"));

        if (attached->path() == ".basea") {
            QVERIFY(delegate);
            QString delegateClass = delegate->metaObject()->className();
            QCOMPARE(delegateClass, QString("QQuickItem"));
        } else if (attached->path() == ".testa") {
            QVERIFY(!delegate);
        } else if (attached->path() == ".testa .base") {
            QVERIFY(delegate);
            QString delegateClass = delegate->metaObject()->className();
            QCOMPARE(delegateClass, QString("QQuickText"));
        }
    }
}


QTEST_MAIN(tst_ThemeEngine)

#include "tst_theme_enginetest.moc"
