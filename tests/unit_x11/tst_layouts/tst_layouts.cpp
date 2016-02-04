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
#include "uctestcase.h"
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
    QString m_modulePath;

public:

    tst_Layouts()
    {
    }

    QQuickView * loadTest(const QString &file)
    {
        UbuntuTestCase* testCase = new UbuntuTestCase(file);
        return qobject_cast<QQuickView*>(testCase);
    }

    QQuickItem *testItem(QQuickItem *that, const QString &identifier)
    {
        if (that->property(identifier.toLocal8Bit()).isValid())
            return that->property(identifier.toLocal8Bit()).value<QQuickItem*>();

        QList<QQuickItem*> children = that->findChildren<QQuickItem*>(identifier);
        return (children.count() > 0) ? children[0] : 0;
    }

    bool hasChildItem(QQuickItem *child, QQuickItem *parent)
    {
        QQuickItem *pl = child->parentItem();
        while (pl) {
            if (pl == parent) {
                return true;
            }
            pl = pl->parentItem();
        }
        return false;
    }

    QQuickItem *prevSibling(QQuickItem *item)
    {
        QList<QQuickItem*> children = item->parentItem()->childItems();
        int index = children.indexOf(item);
        return (index > 0) ? children.at(index - 1) : 0;
    }


private Q_SLOTS:
    void initTestCase()
    {
    }

    void cleanupTestCase()
    {
    }

    void testCase_NoLayouts()
    {
        QScopedPointer<UbuntuTestCase> testCase(new UbuntuTestCase("NoLayouts.qml"));
        ULLayouts *layouts = testCase->findItem<ULLayouts*>("layouts");
        QVERIFY(layouts->layoutList().isEmpty());
    }

    void testCase_SimpleLayout()
    {
        QScopedPointer<QQuickView> view(loadTest("SimpleLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy currentLayout(layouts, SIGNAL(currentLayoutChanged()));
        currentLayout.wait(300);

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
        QScopedPointer<QQuickView> view(loadTest("SimpleLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);

        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy layoutChangeSpy(layouts, SIGNAL(currentLayoutChanged()));

        root->setWidth(UCUnits::instance()->gu(55));
        QCOMPARE(root->width(), UCUnits::instance()->gu(55));
        layoutChangeSpy.wait(100);
        QCOMPARE(layoutChangeSpy.count(), 1);

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
        QScopedPointer<QQuickView> view(loadTest("SimpleLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy layoutChangeSpy(layouts, SIGNAL(currentLayoutChanged()));

        root->setWidth(UCUnits::instance()->gu(65));
        QCOMPARE(root->width(), UCUnits::instance()->gu(65));
        layoutChangeSpy.wait(100);
        QCOMPARE(layoutChangeSpy.count(), 1);

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
        QScopedPointer<QQuickView> view(loadTest("OverlappingCondition.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy currentLayout(layouts, SIGNAL(currentLayoutChanged()));
        currentLayout.wait(300);

        QCOMPARE(layouts->currentLayout(), QString("small"));
    }

    void testCase_ExternalLayouts()
    {
        QScopedPointer<QQuickView> view(loadTest("ExternalLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy currentLayout(layouts, SIGNAL(currentLayoutChanged()));
        currentLayout.wait(300);

        QCOMPARE(layouts->currentLayout(), QString("small"));
    }

    void testCase_ExternalLayouts_Medium()
    {
        QScopedPointer<QQuickView> view(loadTest("ExternalLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy layoutChangeSpy(layouts, SIGNAL(currentLayoutChanged()));

        root->setWidth(UCUnits::instance()->gu(55));
        layoutChangeSpy.wait(100);
        QCOMPARE(layoutChangeSpy.count(), 1);

        QCOMPARE(layouts->currentLayout(), QString("medium"));
    }

    void testCase_ExternalLayouts_Large()
    {
        QScopedPointer<QQuickView> view(loadTest("ExternalLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy layoutChangeSpy(layouts, SIGNAL(currentLayoutChanged()));

        root->setWidth(UCUnits::instance()->gu(65));
        layoutChangeSpy.wait(100);
        QCOMPARE(layoutChangeSpy.count(), 1);

        QCOMPARE(layouts->currentLayout(), QString("large"));
    }

    void testCase_NestedLayouts()
    {
        QScopedPointer<QQuickView> view(loadTest("NestedLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy currentLayout(layouts, SIGNAL(currentLayoutChanged()));
        currentLayout.wait(300);

        QCOMPARE(layouts->currentLayout(), QString("simple"));
    }

    void testCase_NestedLayouts_ExtraLarge()
    {
        QTest::ignoreMessage(QtWarningMsg, "\"There are still \"1\" items in the process of being created at engine destruction.\"");
        QScopedPointer<QQuickView> view(loadTest("NestedLayouts.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layouts"));
        QVERIFY(layouts);
        QVERIFY(!layouts->layoutList().isEmpty());
        QSignalSpy layoutChangeSpy(layouts, SIGNAL(currentLayoutChanged()));

        root->setWidth(UCUnits::instance()->gu(90));
        layoutChangeSpy.wait(100);
        QCOMPARE(layoutChangeSpy.count(), 1);

        QCOMPARE(layouts->currentLayout(), QString("extra-large"));

        root->setWidth(UCUnits::instance()->gu(50));
        layoutChangeSpy.wait(100);
        QCOMPARE(layoutChangeSpy.count(), 2);

        QCOMPARE(root->property("nestedLayout").toString(), QString("medium"));
    }

    void testCase_ResizingContainers()
    {
        QScopedPointer<QQuickView> view(loadTest("ResizingContainers.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
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
        QSignalSpy layoutChangeSpy(layouts, SIGNAL(currentLayoutChanged()));

        root->setWidth(UCUnits::instance()->gu(50));
        layoutChangeSpy.wait(100);
        QCOMPARE(layouts->currentLayout(), QString("small"));
        QCOMPARE(item->width(), UCUnits::instance()->gu(10));
        QCOMPARE(item->height(), UCUnits::instance()->gu(10));

        root->setWidth(UCUnits::instance()->gu(60));
        layoutChangeSpy.wait(100);
        QCOMPARE(layouts->currentLayout(), QString("large"));
        QCOMPARE(item->width(), UCUnits::instance()->gu(22));
        QCOMPARE(item->height(), UCUnits::instance()->gu(22));

        root->setWidth(UCUnits::instance()->gu(80));
        layoutChangeSpy.wait(100);
        QCOMPARE(layouts->currentLayout(), QString("xlarge"));
        QCOMPARE(item->width(), UCUnits::instance()->gu(30));
        QCOMPARE(item->height(), UCUnits::instance()->gu(30));
        QCOMPARE(item2->width(), UCUnits::instance()->gu(40));
        QCOMPARE(item2->height(), UCUnits::instance()->gu(50));

        root->setWidth(UCUnits::instance()->gu(40));
        layoutChangeSpy.wait(100);
        QCOMPARE(item->width(), width);
        QCOMPARE(item->height(), height);
        QCOMPARE(item2->width(), width2);
        QCOMPARE(item2->height(), height2);
    }

    void testCase_CurrentLayoutChange()
    {
        UbuntuTestCase::ignoreWarning("CurrentLayoutChange.qml", 30, 13,
            "QML ConditionalLayout: ERROR: no container specified for layout \"portrait\". ConditionalLayout cannot be activated without a container.");
        UbuntuTestCase::ignoreWarning("CurrentLayoutChange.qml", 35, 13,
            "QML ConditionalLayout: ERROR: no container specified for layout \"landscape\". ConditionalLayout cannot be activated without a container.");
        UbuntuTestCase::ignoreWarning("CurrentLayoutChange.qml", 30, 13,
            "QML ConditionalLayout: WARNING: Cannot activate layout \"portrait\" with no container specified. Falling back to default layout.");
        QScopedPointer<QQuickView> view(loadTest("CurrentLayoutChange.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layoutManager"));
        QVERIFY(layouts);

        QSignalSpy spy(layouts, SIGNAL(currentLayoutChanged()));
        root->setWidth(root->height() - 10);
        spy.wait(100);
        QEXPECT_FAIL(0, "Layout change should not happen when component is not defined", Continue);
        QCOMPARE(spy.count(), 1);
    }

    void testCase_PositioningOnLayoutChange()
    {
        UbuntuTestCase::ignoreWarning("PositioningOnLayoutChange.qml", 42, 13,
            "QML ConditionalLayout: WARNING: Cannot activate layout \"landscape\" with no container specified. Falling back to default layout.", 2);
        UbuntuTestCase::ignoreWarning("PositioningOnLayoutChange.qml", 42, 13,
            "QML ConditionalLayout: ERROR: no container specified for layout \"landscape\". ConditionalLayout cannot be activated without a container.");
        QScopedPointer<QQuickView> view(loadTest("PositioningOnLayoutChange.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layoutManager"));
        QVERIFY(layouts);

        QSignalSpy spy(layouts, SIGNAL(currentLayoutChanged()));
        root->setWidth(root->height() + 10);
        spy.wait(100);
        QEXPECT_FAIL(0, "Layout change should not happen when component is not defined", Continue);
        QCOMPARE(spy.count(), 1);

        spy.clear();
        root->setWidth(root->height() - 10);
        spy.wait(100);
        QCOMPARE(spy.count(), 1);
    }

    void testCase_LaidOutItemsOutsideOfLayout()
    {
        UbuntuTestCase::ignoreWarning("LaidOutItemsOutsideOfLayout.qml", 44, 5,
            "QML Rectangle: ERROR: Item that is not a child of a Layouts component will not be laid out.");
        QScopedPointer<QQuickView> view(loadTest("LaidOutItemsOutsideOfLayout.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
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
        QScopedPointer<QQuickView> view(loadTest("OverlaidInItemLayout.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layouts = qobject_cast<ULLayouts*>(testItem(root, "layoutManager"));
        QVERIFY(layouts);
        QSignalSpy currentLayout(layouts, SIGNAL(currentLayoutChanged()));
        currentLayout.wait(300);

        QQuickItem *layout = qobject_cast<QQuickItem*>(testItem(root, "layout"));
        QVERIFY(layout);

        QQuickItem *item = qobject_cast<QQuickItem*>(testItem(root, "hostedItem"));
        QVERIFY(item);

        QCOMPARE(layout->childItems()[0], item);
    }

    void testCase_AnchorFilledReparenting()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorFilledReparenting.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->fill(), layout->contentItem());
    }

    void testCase_AnchorFilledMargins()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorFilledMargins.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);

        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->margins(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->fill(), layout->contentItem());
        QCOMPARE(anchors->margins(), 10.0);
    }

    void testCase_AnchorFilledSeparateMargins()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorFilledSeparateMargins.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);

        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->margins(), 0.0);
        QCOMPARE(anchors->leftMargin(), 0.0);
        QCOMPARE(anchors->rightMargin(), 0.0);
        QCOMPARE(anchors->topMargin(), 0.0);
        QCOMPARE(anchors->bottomMargin(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->fill(), layout->contentItem());
        QCOMPARE(anchors->leftMargin(), 10.0);
        QCOMPARE(anchors->topMargin(), 20.0);
        QCOMPARE(anchors->rightMargin(), 30.0);
        QCOMPARE(anchors->bottomMargin(), 40.0);
    }

    void testCase_AnchorCenteredInDefault()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorCenteredInDefault.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->centerIn(), layout->contentItem());
    }

    void testCase_AnchorVerticalCenter()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorVerticalCenter.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->verticalCenter().item, layout->contentItem());

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->verticalCenter().item, layout->contentItem());
    }

    void testCase_AnchorVerticalCenterOffset()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorVerticalCenterOffset.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->verticalCenter().item, layout->contentItem());
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        // no need to check offset as it does not affect the fill

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->verticalCenter().item, layout->contentItem());
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);
    }

    void testCase_AnchorHorizontalCenter()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorHorizontalCenter.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->horizontalCenter().item, layout->contentItem());

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->horizontalCenter().item, layout->contentItem());
    }

    void testCase_AnchorHorizontalCenterOffset()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorHorizontalCenterOffset.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->horizontalCenter().item, layout->contentItem());
        QCOMPARE(anchors->horizontalCenterOffset(), 50.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        // no need to check offset as it does not affect the fill

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->horizontalCenter().item, layout->contentItem());
        QCOMPARE(anchors->horizontalCenterOffset(), 50.0);
    }

    void testCase_AnchorCenterWithOffset()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorCenterWithOffset.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->centerIn(), layout->contentItem());
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);
        QCOMPARE(anchors->horizontalCenterOffset(), 40.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        // no need to check offsets as it does not affect the fill

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->centerIn(), layout->contentItem());
        QCOMPARE(anchors->verticalCenterOffset(), 50.0);
        QCOMPARE(anchors->horizontalCenterOffset(), 40.0);
    }

    void testCase_AnchorLeft()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorLeft.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->left().item, layout->contentItem());
        QCOMPARE(anchors->leftMargin(), 10.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->leftMargin(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->left().item, layout->contentItem());
        QCOMPARE(anchors->leftMargin(), 10.0);
    }

    void testCase_AnchorTop()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorTop.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->top().item, layout->contentItem());
        QCOMPARE(anchors->topMargin(), 10.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->topMargin(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->top().item, layout->contentItem());
        QCOMPARE(anchors->topMargin(), 10.0);
    }

    void testCase_AnchorRight()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorRight.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->right().item, layout->contentItem());
        QCOMPARE(anchors->rightMargin(), 10.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->rightMargin(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->right().item, layout->contentItem());
        QCOMPARE(anchors->rightMargin(), 10.0);
    }

    void testCase_AnchorBottom()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorBottom.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->bottom().item, layout->contentItem());
        QCOMPARE(anchors->bottomMargin(), 10.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QCOMPARE(anchors->bottomMargin(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->bottom().item, layout->contentItem());
        QCOMPARE(anchors->bottomMargin(), 10.0);
    }

    void testCase_AnchorAll()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("AnchorAll.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *item = view->findItem<QQuickItem*>("testItem");
        QVERIFY(item);

        QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors*>();
        QVERIFY(anchors);

        QCOMPARE(anchors->left().item, layout->contentItem());
        QCOMPARE(anchors->top().item, layout->contentItem());
        QCOMPARE(anchors->right().item, layout->contentItem());
        QCOMPARE(anchors->bottom().item, layout->contentItem());
        QVERIFY(!anchors->fill());
        QCOMPARE(anchors->margins(), 20.0);

        root->setWidth(root->width() + 100);
        layoutChangeSpy.wait(100);
        QQuickItem *testLayout = view->findItem<QQuickItem*>("testLayout");
        QVERIFY(testLayout);
        QCOMPARE(anchors->fill(), testLayout);
        QVERIFY(!anchors->left().item);
        QVERIFY(!anchors->top().item);
        QVERIFY(!anchors->right().item);
        QVERIFY(!anchors->bottom().item);
        QCOMPARE(anchors->margins(), 0.0);

        root->setWidth(root->width() - 100);
        layoutChangeSpy.wait(100);
        QCOMPARE(anchors->left().item, layout->contentItem());
        QCOMPARE(anchors->top().item, layout->contentItem());
        QCOMPARE(anchors->right().item, layout->contentItem());
        QCOMPARE(anchors->bottom().item, layout->contentItem());
        QVERIFY(!anchors->fill());
        QCOMPARE(anchors->margins(), 20.0);
    }

    // the following tests guard bug #1280359
    void testCase_ItemInstanceAsProperty()
    {
        UbuntuTestCase::ignoreWarning("ItemInstanceAsProperty.qml", 45, 17,
            "QML ItemLayout: WARNING: item \"object2\" not specified or has been specified for layout by more than one active ItemLayout");
        QScopedPointer<QQuickView> view(loadTest("ItemInstanceAsProperty.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        QQuickItem *layout = testItem(root, "layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        // invoke layout change
        view->rootObject()->metaObject()->invokeMethod(view->rootObject(), "changeLayout");
        layoutChangeSpy.wait();
        QCOMPARE(layoutChangeSpy.count(), 1);
    }

    void testCase_DialerCrash()
    {
        UbuntuTestCase::ignoreWarning("DialerCrash.qml", 45, 17,
            "QML ItemLayout: WARNING: item \"object2\" not specified or has been specified for layout by more than one active ItemLayout");
        QScopedPointer<QQuickView> view(loadTest("DialerCrash.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        QQuickItem *layout = testItem(root, "layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        // invoke layout change
        view->rootObject()->setProperty("wide", false);
        layoutChangeSpy.wait();
        QCOMPARE(layoutChangeSpy.count(), 1);
    }

    void testCase_ExcludedItemDeleted()
    {
        QScopedPointer<QQuickView> view(loadTest("ExcludedItemDeleted.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        QQuickItem *layout = testItem(root, "layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        // invoke layout change
        view->rootObject()->metaObject()->invokeMethod(view->rootObject(), "changeLayout");
        layoutChangeSpy.wait();
        QCOMPARE(layoutChangeSpy.count(), 1);
    }

    // guard bug #1204834 and #1300668
    void testCase_Visibility()
    {
        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("Visibility.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *layout = view->findItem<ULLayouts*>("layoutManager");
        QVERIFY(layout);
        QSignalSpy layoutChangeSpy(layout, SIGNAL(currentLayoutChanged()));

        QQuickItem *defaultLayout = testItem(root, "DefaultLayout");
        QVERIFY(defaultLayout);

        // invoke layout change
        view->rootObject()->metaObject()->invokeMethod(view->rootObject(), "portraitLayout");
        layoutChangeSpy.wait(1000);
        QCOMPARE(defaultLayout->parentItem(), layout->contentItem());
        // despite of visible property binding, the visibility of the component is false
        // while its parent is set to invisible
        QCOMPARE(defaultLayout->isVisible(), false);
        QCOMPARE(layout->contentItem()->isVisible(), false);

        // change layout back
        layoutChangeSpy.clear();
        view->rootObject()->metaObject()->invokeMethod(view->rootObject(), "landscapeLayout");
        layoutChangeSpy.wait(1000);
        QCOMPARE(defaultLayout->parentItem(), layout->contentItem());
        QCOMPARE(defaultLayout->isVisible(), true);
        QCOMPARE(layout->contentItem()->isVisible(), true);
    }

    void testCase_NestedVisibility_data() {
        QTest::addColumn<QString>("layoutFunction");
        QTest::addColumn<QString>("layoutName");

        QTest::newRow("portrait") << "tabletPortraitLayout" << "tabletPortrait";
        QTest::newRow("landscape") << "tabletLandscapeLayout" << "tabletLandscape";
    }

    void testCase_NestedVisibility()
    {
        QFETCH(QString, layoutFunction);
        QFETCH(QString, layoutName);

        QScopedPointer<UbuntuTestCase> view(new UbuntuTestCase("NestedVisibility.qml"));
        QVERIFY(view);
        QQuickItem *root = view->rootObject();
        QVERIFY(root);

        ULLayouts *mainLayout = view->findItem<ULLayouts*>("mainLayout");
        QVERIFY(mainLayout);
        QQuickItem *red = testItem(mainLayout, "mainRed");
        QVERIFY(red);
        QQuickItem *green = testItem(mainLayout, "mainGreen");
        QVERIFY(green);
        QQuickItem *blue = testItem(mainLayout, "mainBlue");
        QVERIFY(blue);
        QQuickItem *magenta = testItem(mainLayout, "mainMagenta");
        QVERIFY(magenta);
        QQuickItem *hidden = testItem(mainLayout, "mainHidden");
        QVERIFY(hidden);
        QCOMPARE(hidden->isVisible(), false);
        // default is phone layout, get layd out items' neighbors
        QQuickItem *greenPrev = prevSibling(green);
        QQuickItem *magentaPrev = prevSibling(magenta);
        QSizeF greenSize(green->width(), green->height());
        QSizeF magentaSize(magenta->width(), magenta->height());

        // invoke tablet layout change
        QSignalSpy mainLayoutChangeSpy(mainLayout, SIGNAL(currentLayoutChanged()));
        view->rootObject()->metaObject()->invokeMethod(view->rootObject(), layoutFunction.toLocal8Bit().data());
        mainLayoutChangeSpy.wait(1000);
        // get the nested layout
        ULLayouts *nestedLayout = qobject_cast<ULLayouts*>(testItem(mainLayout, "nestedLayout"));
        QVERIFY(nestedLayout);
        QQuickItem *nestedActiveLayout = testItem(nestedLayout, layoutName);
        QVERIFY(nestedActiveLayout);
        QCOMPARE(red->isVisible(), false);
        QCOMPARE(green->isVisible(), true);
        QCOMPARE(blue->isVisible(), false);
        QCOMPARE(magenta->isVisible(), true);
        QCOMPARE(hidden->isVisible(), false);
        // we cannot use findChild() to locate green and magenta boxes, as reparenting only
        // changes the parentItem, not the parent object itself; therefore we only check if
        // their container is in the layout
        QVERIFY(hasChildItem(green, nestedActiveLayout));
        QVERIFY(hasChildItem(magenta, nestedActiveLayout));

        // go back to default layout of main
        mainLayoutChangeSpy.clear();
        view->rootObject()->metaObject()->invokeMethod(view->rootObject(), "phoneLayout");
        mainLayoutChangeSpy.wait(1000);
        QCOMPARE(red->isVisible(), true);
        QCOMPARE(green->isVisible(), true);
        QCOMPARE(blue->isVisible(), true);
        QCOMPARE(magenta->isVisible(), true);
        QCOMPARE(hidden->isVisible(), false);
        QCOMPARE(prevSibling(green), greenPrev);
        QCOMPARE(prevSibling(magenta), magentaPrev);
        QCOMPARE(green->width(), greenSize.width());
        QCOMPARE(green->height(), greenSize.height());
        QCOMPARE(magenta->width(), magentaSize.width());
        QCOMPARE(magenta->height(), magentaSize.height());
        QVERIFY(hasChildItem(green, mainLayout->contentItem()));
        QVERIFY(hasChildItem(magenta, mainLayout->contentItem()));
    }

};

QTEST_MAIN(tst_Layouts)

#include "tst_layouts.moc"
