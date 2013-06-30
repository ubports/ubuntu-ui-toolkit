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
#include <QtCore/QCoreApplication>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "ullayouts.h"
#include "ucunits.h"
#include <QtQuick/private/qquickanchors_p.h>
#include <QtQuick/private/qquickanchors_p_p.h>

#define QCOMPARE_RET(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return false;\
} while (0)

#define QVERIFY_RET(statement, resturns) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return resturns;\
} while (0)


class tst_Layouts : public QObject
{
    Q_OBJECT
private:
    QQuickView *view;

public:

    tst_Layouts() :
        view(0)
    {
    }

    QQuickItem *loadTest(const QString &document)
    {
        // load the document
        view->setSource(QUrl::fromLocalFile(document));
        QTest::waitForEvents();

        return view->rootObject();
    }

    QQuickItem *testItem(QQuickItem *that, const QString &identifier)
    {
        if (that->property(identifier.toLocal8Bit()).isValid())
            return that->property(identifier.toLocal8Bit()).value<QQuickItem*>();

        QList<QQuickItem*> children = that->findChildren<QQuickItem*>(identifier);
        return (children.count() > 0) ? children[0] : 0;
    }


private Q_SLOTS:
    void initTestCase()
    {
        QString modules("../../../modules");
        QVERIFY(QDir(modules).exists());

        view = new QQuickView;
        QQmlEngine *quickEngine = view->engine();

        view->setGeometry(0,0, UCUnits::instance().gu(40), UCUnits::instance().gu(30));
        //add modules folder so we have access to the plugin from QML
        QStringList imports = quickEngine->importPathList();
        imports.prepend(QDir(modules).absolutePath());
        quickEngine->setImportPathList(imports);
    }

    void cleanupTestCase()
    {
        delete view;
    }

    void testCase_NoLayouts()
    {
        QQuickItem *root = loadTest("NoLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);

        QVERIFY(layouts->layoutList().isEmpty());
    }

    void testCase_SimpleLayout()
    {
        QQuickItem *root = loadTest("SimpleLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());

        QCOMPARE(layouts->currentLayout(), QString("small"));

        QQuickItem *item = testItem(root, "item1");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickColumn"));

        item = testItem(root, "item2");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickColumn"));

        item = testItem(root, "item3");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickColumn"));
    }

    void testCase_SimpleLayout_Medium()
    {
        QQuickItem *root = loadTest("SimpleLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        root->setWidth(UCUnits::instance().gu(55));
        QCOMPARE(root->width(), UCUnits::instance().gu(55));
        QTest::waitForEvents();

        QCOMPARE(layouts->currentLayout(), QString("medium"));

        QQuickItem *item = testItem(root, "item1");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickFlow"));

        item = testItem(root, "item2");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickFlow"));

        item = testItem(root, "item3");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickFlow"));
    }

    void testCase_SimpleLayout_Large()
    {
        QQuickItem *root = loadTest("SimpleLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        root->setWidth(UCUnits::instance().gu(65));
        QCOMPARE(root->width(), UCUnits::instance().gu(65));
        QTest::waitForEvents();

        QCOMPARE(layouts->currentLayout(), QString("large"));

        QQuickItem *item = testItem(root, "item1");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickRow"));

        item = testItem(root, "item2");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickRow"));

        item = testItem(root, "item3");
        QVERIFY(item);
        QVERIFY(item->parentItem()->parentItem()->inherits("QQuickRow"));
    }

    void testCase_OverlappingCondition()
    {
        QQuickItem *root = loadTest("OverlappingCondition.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());

        QCOMPARE(layouts->currentLayout(), QString("small"));
    }

    void testCase_ExternalLayouts()
    {
        QQuickItem *root = loadTest("ExternalLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());

        QCOMPARE(layouts->currentLayout(), QString("small"));
    }

    void testCase_ExternalLayouts_Medium()
    {
        QQuickItem *root = loadTest("ExternalLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        root->setWidth(UCUnits::instance().gu(55));

        QCOMPARE(layouts->currentLayout(), QString("medium"));
    }

    void testCase_ExternalLayouts_Large()
    {
        QQuickItem *root = loadTest("ExternalLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        root->setWidth(UCUnits::instance().gu(65));

        QCOMPARE(layouts->currentLayout(), QString("large"));
    }

    void testCase_NestedLayouts()
    {
        QQuickItem *root = loadTest("NestedLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());

        QCOMPARE(layouts->currentLayout(), QString("simple"));
    }

    void testCase_NestedLayouts_ExtraLarge()
    {
        QQuickItem *root = loadTest("NestedLayouts.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        root->setWidth(UCUnits::instance().gu(90));
        QTest::waitForEvents();

        QCOMPARE(layouts->currentLayout(), QString("extra-large"));

        root->setWidth(UCUnits::instance().gu(50));
        QTest::waitForEvents();

        QCOMPARE(root->property("nestedLayout").toString(), QString("medium"));
    }

    void testCase_ResizingContainers()
    {
        QQuickItem *root = loadTest("ResizingContainers.qml");
        QVERIFY(root);

        // fetch the current size of one item
        QQuickItem *item = testItem(root, "item1");
        qreal width = item->width();
        qreal height = item->height();
        QQuickItem *item2 = testItem(root, "item2");
        qreal width2 = item2->width();
        qreal height2 = item2->height();

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());

        root->setWidth(UCUnits::instance().gu(50));
        QTest::waitForEvents();
        QCOMPARE(layouts->currentLayout(), QString("small"));
        QCOMPARE(item->width(), UCUnits::instance().gu(10));
        QCOMPARE(item->height(), UCUnits::instance().gu(10));

        root->setWidth(UCUnits::instance().gu(60));
        QTest::waitForEvents();
        QCOMPARE(layouts->currentLayout(), QString("large"));
        QCOMPARE(item->width(), UCUnits::instance().gu(22));
        QCOMPARE(item->height(), UCUnits::instance().gu(22));

        root->setWidth(UCUnits::instance().gu(80));
        QTest::waitForEvents();
        QCOMPARE(layouts->currentLayout(), QString("xlarge"));
        QCOMPARE(item->width(), UCUnits::instance().gu(30));
        QCOMPARE(item->height(), UCUnits::instance().gu(30));
        QCOMPARE(item2->width(), UCUnits::instance().gu(40));
        QCOMPARE(item2->height(), UCUnits::instance().gu(50));

        root->setWidth(UCUnits::instance().gu(40));
        QTest::waitForEvents();
        QCOMPARE(item->width(), width);
        QCOMPARE(item->height(), height);
        QCOMPARE(item2->width(), width2);
        QCOMPARE(item2->height(), height2);
    }

    void testCase_CurrentLayoutChange()
    {
        QQuickItem *root = loadTest("CurrentLayoutChange.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layoutManager"));
        QVERIFY(layouts);

        QSignalSpy spy(layouts, SIGNAL(currentLayoutChanged()));
        root->setWidth(root->height() - 10);
        QEXPECT_FAIL(0, "Layout change should not happen when component is not defined", Continue);
        QCOMPARE(spy.count(), 1);
    }

    void testCase_PositioningOnLayoutChange()
    {
        QQuickItem *root = loadTest("PositioningOnLayoutChange.qml");
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layoutManager"));
        QVERIFY(layouts);

        QSignalSpy spy(layouts, SIGNAL(currentLayoutChanged()));
        root->setWidth(root->height() - 10);
        QEXPECT_FAIL(0, "Layout change should not happen when component is not defined", Continue);
        QCOMPARE(spy.count(), 1);

        spy.clear();
        root->setWidth(root->height() + 10);
        QCOMPARE(spy.count(), 1);
    }

    void testCase_LaidOutItemsOutsideOfLayout()
    {
        QQuickItem *root = loadTest("LaidOutItemsOutsideOfLayout.qml");
        QVERIFY(root);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "itemLaidOut"));
        QVERIFY(item);

        ULLayoutsAttached *marker = qobject_cast<ULLayoutsAttached*>(
                    qmlAttachedPropertiesObject<ULLayouts>(item, false));
        QVERIFY(marker);
        QVERIFY(!marker->isValid());
    }

    void testCase_OverlaidInItemLayout()
    {
        QQuickItem *root = loadTest("OverlaidInItemLayout.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layout"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "hostedItem"));
        QVERIFY(item);

        QCOMPARE(layout->childItems()[0], item);
    }

    void testCase_AnchorFilledReparenting()
    {
        QQuickItem *root = loadTest("AnchorFilledReparenting.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->fill(), layout);
    }

    void testCase_AnchorFilledMargins()
    {
        QQuickItem *root = loadTest("AnchorFilledMargins.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();

        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->margins(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->fill(), layout);
        QCOMPARE(anchors->margins(), 10.0);
    }

    void testCase_AnchorFilledSeparateMargins()
    {
        QQuickItem *root = loadTest("AnchorFilledSeparateMargins.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();

        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->margins(), 0.0);
        QCOMPARE(anchors->leftMargin(), 0.0);
        QCOMPARE(anchors->rightMargin(), 0.0);
        QCOMPARE(anchors->topMargin(), 0.0);
        QCOMPARE(anchors->bottomMargin(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->fill(), layout);
        QCOMPARE(anchors->leftMargin(), 10.0);
        QCOMPARE(anchors->topMargin(), 20.0);
        QCOMPARE(anchors->rightMargin(), 30.0);
        QCOMPARE(anchors->bottomMargin(), 40.0);
    }

    void testCase_AnchorCenteredInDefault()
    {
        QQuickItem *root = loadTest("AnchorCenteredInDefault.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->centerIn(), layout);
    }

    void testCase_AnchorVerticalCenter()
    {
        QQuickItem *root = loadTest("AnchorVerticalCenter.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->verticalCenter().item, layout);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->verticalCenter().item, layout);
    }

    void testCase_AnchorVerticalCenterOffset()
    {
        QQuickItem *root = loadTest("AnchorVerticalCenterOffset.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->verticalCenter().item, layout);
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        // no need to check offset as it does not affect the fill

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->verticalCenter().item, layout);
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);
    }

    void testCase_AnchorHorizontalCenter()
    {
        QQuickItem *root = loadTest("AnchorHorizontalCenter.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->horizontalCenter().item, layout);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->horizontalCenter().item, layout);
    }

    void testCase_AnchorHorizontalCenterOffset()
    {
        QQuickItem *root = loadTest("AnchorHorizontalCenterOffset.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->horizontalCenter().item, layout);
        QCOMPARE(anchors->horizontalCenterOffset(), 50.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        // no need to check offset as it does not affect the fill

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->horizontalCenter().item, layout);
        QCOMPARE(anchors->horizontalCenterOffset(), 50.0);
    }

    void testCase_AnchorCenterWithOffset()
    {
        QQuickItem *root = loadTest("AnchorCenterWithOffset.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->centerIn(), layout);
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);
        QCOMPARE(anchors->horizontalCenterOffset(), 40.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        // no need to check offsets as it does not affect the fill

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->centerIn(), layout);
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);
        QCOMPARE(anchors->horizontalCenterOffset(), 40.0);
    }

    void testCase_AnchorLeft()
    {
        QQuickItem *root = loadTest("AnchorLeft.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->left().item, layout);
        QCOMPARE(anchors->leftMargin(), 10.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->leftMargin(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->left().item, layout);
        QCOMPARE(anchors->leftMargin(), 10.0);
    }

    void testCase_AnchorTop()
    {
        QQuickItem *root = loadTest("AnchorTop.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->top().item, layout);
        QCOMPARE(anchors->topMargin(), 10.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->topMargin(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->top().item, layout);
        QCOMPARE(anchors->topMargin(), 10.0);
    }

    void testCase_AnchorRight()
    {
        QQuickItem *root = loadTest("AnchorRight.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->right().item, layout);
        QCOMPARE(anchors->rightMargin(), 10.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->rightMargin(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->right().item, layout);
        QCOMPARE(anchors->rightMargin(), 10.0);
    }

    void testCase_AnchorBottom()
    {
        QQuickItem *root = loadTest("AnchorBottom.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->bottom().item, layout);
        QCOMPARE(anchors->bottomMargin(), 10.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->bottomMargin(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->bottom().item, layout);
        QCOMPARE(anchors->bottomMargin(), 10.0);
    }

    void testCase_AnchorAll()
    {
        QQuickItem *root = loadTest("AnchorAll.qml");
        QVERIFY(root);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layoutManager"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "testItem"));
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->left().item, layout);
        QCOMPARE(anchors->top().item, layout);
        QCOMPARE(anchors->right().item, layout);
        QCOMPARE(anchors->bottom().item, layout);
        QVERIFY(!anchors->fill());
        QCOMPARE(anchors->margins(), 20.0);

        root->setWidth(root->width() + 100);
        QTest::waitForEvents();
        QQuickItem *testLayout = qobject_cast<QQuickItem*>(testItem(root, "testLayout"));
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QVERIFY(!anchors->left().item);
        QVERIFY(!anchors->top().item);
        QVERIFY(!anchors->right().item);
        QVERIFY(!anchors->bottom().item);
        QCOMPARE(anchors->margins(), 0.0);

        root->setWidth(root->width() - 100);
        QTest::waitForEvents();
        QCOMPARE(anchors->left().item, layout);
        QCOMPARE(anchors->top().item, layout);
        QCOMPARE(anchors->right().item, layout);
        QCOMPARE(anchors->bottom().item, layout);
        QVERIFY(!anchors->fill());
        QCOMPARE(anchors->margins(), 20.0);
    }

};

QTEST_MAIN(tst_Layouts)

#include "tst_layouts.moc"
