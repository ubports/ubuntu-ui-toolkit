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
#include "itemstyleattached.h"

#define QCOMPARE_RET(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return false;\
} while (0)

#define QVERIFY_RET(statement) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return false;\
} while (0)


class tst_ThemeEngine : public QObject
{
    Q_OBJECT

public:
    tst_ThemeEngine();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testCase_initializeEngine();
    void testCase_loadTheme();
    void testCase_reparenting();
    void testCase_blockDeclaration();
    void testCase_selectorDelegates();
    void testCase_inheritance();
    void testCase_NonStyledParent();
    void testCase_NonStyledRepeaterParent();
    void testCase_NonStyledListViewParent();
    void testCase_ReParentNonStyledToStyled();
    void testCase_ReParentNonStyledToNonStyled();
    void testCase_ReParentNonStyledToStyledTwice();
    void testCase_ReParentNonStyledToNonStyledTwice();
private:
    bool check_properties(QObject *style, const QString &properties, bool xfail);
    QQuickItem *loadTest(const QString &document, const QUrl &theme = QUrl());
    QQuickItem *testItem(QQuickItem *that, const QString &identifier);
private:
    QQuickView *quickView;
    QQmlEngine *quickEngine;
};

tst_ThemeEngine::tst_ThemeEngine():
    quickView(0),
    quickEngine(0)
{
}

QQuickItem *tst_ThemeEngine::loadTest(const QString &document, const QUrl &theme)
{
    QObject *root = quickView->rootObject();

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
    QTest::waitForEvents();
    //QCoreApplication::processEvents();

    return quickView->rootObject();
}

QQuickItem *tst_ThemeEngine::testItem(QQuickItem *that, const QString &identifier)
{
    if (that->property(identifier.toLocal8Bit()).isValid())
        return that->property(identifier.toLocal8Bit()).value<QQuickItem*>();

    QList<QQuickItem*> children = that->findChildren<QQuickItem*>();
    Q_FOREACH(QQuickItem *child, children) {
        if (child->objectName() == identifier) {
            return child;
        } else {
            return testItem(child, identifier);
        }
    }
    return 0;
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

void tst_ThemeEngine::testCase_inheritance()
{
    ThemeEngine::initializeEngine(quickEngine);
    ThemeEngine::instance()->resetError();
    ThemeEngine::instance()->loadTheme(QUrl::fromLocalFile("../../resources/inheritance.qmltheme"));
    quickView->setSource(QUrl::fromLocalFile("InheritanceTest.qml"));
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
        QVERIFY2(attached, "No attached style");
        QObject *style = qvariant_cast<QObject*>(attached->property("style"));

        if (attached->path() == ".derivate.basea")
            QVERIFY(check_properties(style, "pDerivate:pDerivate,pBaseA:pBaseA", false));
        if (attached->path() == ".derivate2.derivate")
            QVERIFY(!check_properties(style, "pDerivate:pDerivate,pBaseA:derivate2", true));
        if (attached->path() == ".multiple.basea.baseb")
            QVERIFY(check_properties(style, "pBaseA:pBaseA,pBaseB:pBaseB", false));
        if (attached->path() == ".multiple2.basea.derivate")
            QVERIFY(!check_properties(style, "pBaseA:pBaseA,pDerivate:pDerivate,pMultiple2:multiple2", true));
        if (attached->path() == ".multiple3.derivate2.baseb")
            QVERIFY(!check_properties(style, "pDerivate:multiple3,pBaseA:derivate2,pBaseB:pBaseB", true));
        if (attached->path() == ".restore.derivate2.basea")
            QVERIFY(!check_properties(style, "pBaseA:pBaseA,pDerivate:pDerivate", true));
    }
}

bool tst_ThemeEngine::check_properties(QObject *style, const QString &properties, bool xfail)
{
    if (!xfail || (xfail && style))
        QVERIFY_RET(style);
    if (style) {
        Q_FOREACH(const QString &propertyPair, properties.split(',')) {
            QStringList pair = propertyPair.split(':');
            QString data = style->property(pair[0].toLatin1()).toString();
            if (!xfail || (xfail && !data.isEmpty()))
                QCOMPARE_RET(data, pair[1]);
        }
    }
    return !xfail;
}

void tst_ThemeEngine::testCase_NonStyledParent()
{
    QQuickItem *root = loadTest("NonStyledParent.qml", QUrl::fromLocalFile("Reparenting1.qmltheme"));
    QVERIFY(root);
    QQuickItem *item = testItem(root, "testItem");
    QVERIFY(item);
    QObject *object = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
    ItemStyleAttached *styleItem = qobject_cast<ItemStyleAttached*>(object);
    QVERIFY(styleItem);
    // verify item path
    QCOMPARE(styleItem->path(), QString(".parent .label"));
}

void tst_ThemeEngine::testCase_NonStyledRepeaterParent()
{
    QQuickItem *root = loadTest("NonStyledRepeaterParent.qml", QUrl::fromLocalFile("Reparenting1.qmltheme"));
    QVERIFY(root);
    QQuickItem *item = testItem(root, "testItem");
    QVERIFY(item);
    QObject *object = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
    ItemStyleAttached *styleItem = qobject_cast<ItemStyleAttached*>(object);
    QVERIFY(styleItem);
    // verify item path
    QCOMPARE(styleItem->path(), QString(".parent .label"));
}

void tst_ThemeEngine::testCase_NonStyledListViewParent()
{
    QQuickItem *root = loadTest("NonStyledListViewParent.qml", QUrl::fromLocalFile("Reparenting1.qmltheme"));
    QVERIFY(root);
    QQuickItem *item = testItem(root, "testItem");
    QVERIFY(item);
    QObject *object = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
    ItemStyleAttached *styleItem = qobject_cast<ItemStyleAttached*>(object);
    QVERIFY(styleItem);
    // verify item path
    QCOMPARE(styleItem->path(), QString(".parent .label"));
}

void tst_ThemeEngine::testCase_ReParentNonStyledToStyled()
{
    QQuickItem *root = loadTest("ReParentNonStyledToStyled.qml", QUrl::fromLocalFile("Reparenting1.qmltheme"));
    QVERIFY(root);
    QQuickItem *item = testItem(root, "testItem");
    QVERIFY(item);
    QObject *object = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
    ItemStyleAttached *styleItem = qobject_cast<ItemStyleAttached*>(object);
    QVERIFY(styleItem);
    // verify item path
    QCOMPARE(styleItem->path(), QString(".parent .label"));
}

void tst_ThemeEngine::testCase_ReParentNonStyledToNonStyled()
{
    QQuickItem *root = loadTest("ReParentNonStyledToNonStyled.qml", QUrl::fromLocalFile("Reparenting1.qmltheme"));
    QVERIFY(root);
    QQuickItem *item = testItem(root, "testItem");
    QVERIFY(item);
    QObject *object = qmlAttachedPropertiesObject<ItemStyleAttached>(item, false);
    ItemStyleAttached *styleItem = qobject_cast<ItemStyleAttached*>(object);
    QVERIFY(styleItem);
    // verify item path
    QCOMPARE(styleItem->path(), QString(".parent .label"));
}

void tst_ThemeEngine::testCase_ReParentNonStyledToStyledTwice()
{
    QQuickItem *root = loadTest("ReParentNonStyledToStyledTwice.qml", QUrl::fromLocalFile("Reparenting2.qmltheme"));
    QVERIFY(root);

    QString path = root->property("path1").toString();
    QCOMPARE(path, QString(".parent .label"));
    path = root->property("path2").toString();
    QCOMPARE(path, QString(".other-parent .label"));
}

void tst_ThemeEngine::testCase_ReParentNonStyledToNonStyledTwice()
{
    QQuickItem *root = loadTest("ReParentNonStyledToNonStyledTwice.qml", QUrl::fromLocalFile("Reparenting2.qmltheme"));
    QVERIFY(root);

    QString path = root->property("path1").toString();
    QCOMPARE(path, QString(".parent .label"));
    path = root->property("path2").toString();
    QCOMPARE(path, QString(".other-parent .label"));
}

QTEST_MAIN(tst_ThemeEngine)

#include "tst_theme_enginetest.moc"
