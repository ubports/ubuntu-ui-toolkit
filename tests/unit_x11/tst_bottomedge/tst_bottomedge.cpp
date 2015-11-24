/*
 * Copyright 2015 Canonical Ltd.
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

#include <QtTest/QtTest>
#include "ucbottomedge.h"
#include "ucbottomedgeregion.h"
#include "ucbottomedge_p.h"
#include "ucbottomedgehint.h"
#include "gestures/ucswipearea.h"
#include "uctestcase.h"
#include "uctestextras.h"
#define private public
#include "quickutils.h"
#include "ucbottomedgestyle.h"
#undef private

#define QVERIFY_RETURN(statement, returnValue) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return returnValue;\
} while (0)

class BottomEdgeTestCase : public UbuntuTestCase
{
    Q_OBJECT
public:
    BottomEdgeTestCase(const QString& file, ResizeMode resize = SizeViewToRootObject, bool assertOnFailure = true, QWindow* parent = 0)
        : UbuntuTestCase(file, resize, assertOnFailure, parent)
    {
        // patch all BottomEdges' SwipeArea gesture recognition timer
        QList<UCBottomEdge*> list = findChildren<UCBottomEdge*>();
        for (int i = 0; i < list.size(); ++i) {
            UCBottomEdge *bottomEdge = list[i];
            UCSwipeArea *swipeArea = bottomEdge->hint()->swipeArea();
            swipeArea->setImmediateRecognition(true);
        }
    }
    ~BottomEdgeTestCase()
    {
        // add a small timeout after each run to have a proper cleanup
        QTest::qWait(400);
    }

    UCBottomEdge *testItem(const QString &objectName = "testItem")
    {
        return findItem<UCBottomEdge*>(objectName);
    }

    typedef QList<UCBottomEdgeRegion*> RegionList;
    RegionList *regions(const QString &testItem)
    {
        QQmlListProperty<UCBottomEdgeRegion> qmlRegions(this->testItem(testItem)->regions());
        return reinterpret_cast<RegionList*>(qmlRegions.data);
    }

    UCBottomEdgeRegion *regionAt(const QString &testItem, int index)
    {
        QVERIFY_RETURN(regions(testItem), nullptr);
        QVERIFY_RETURN(regions(testItem)->size() > index, nullptr);
        return regions(testItem)->at(index);
    }
};


class tst_BottomEdge : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void initTestCase()
    {
        UCTestExtras::registerTouchDevice();
        // make sure we disable the mouse
        QuickUtils::instance().m_mouseAttached = false;
    }

    void test_defaults()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("Defaults.qml"));
        QCOMPARE(test->testItem()->height(), test->rootObject()->height());
        QCOMPARE(test->testItem()->dragProgress(), 0.0);
        QCOMPARE(test->testItem()->dragDirection(), UCBottomEdge::Undefined);
        QCOMPARE(test->testItem()->status(), UCBottomEdge::Hidden);
        QCOMPARE(test->testItem()->contentUrl(), QUrl());
        QVERIFY(!test->testItem()->contentComponent());
        QVERIFY(!test->testItem()->contentItem());
        QVERIFY(test->regions("testItem"));
        QCOMPARE(test->regions("testItem")->size(), 1);
        QCOMPARE(test->regionAt("testItem", 0)->objectName(), QString("default_BottomEdgeRegion"));
        QCOMPARE(test->regionAt("testItem", 0)->m_from, 0.33);
        QCOMPARE(test->regionAt("testItem", 0)->m_to, 1.0);
        QVERIFY(!test->testItem()->activeRegion());
    }

    void test_height_moves_when_reparented()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("DifferentSizes.qml"));
        QQuickItem *newParent = test->rootObject();
        QQuickItem *oldParent = test->findItem<QQuickItem*>("oldParent");
        UCBottomEdge *testItem = test->testItem();

        QSignalSpy spy(testItem, SIGNAL(implicitHeightChanged()));
        testItem->setParentItem(newParent);
        UbuntuTestCase::waitForSignal(&spy);

        // change the implicit height so we are sure we don't get the height change triggered
        testItem->setImplicitHeight(0);
        spy.clear();
        oldParent->setHeight(20);
        QEXPECT_FAIL(0, "no implicitHeight change is expected", Continue);
        QVERIFY(spy.wait(400));
    }

    void test_panel_is_last_item_of_parent()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("LastItem.qml"));
        QQuickItem *bottomEdgeParent = test->testItem()->parentItem();
        UCBottomEdgePrivate *privateTestItem = UCBottomEdgePrivate::get(test->testItem());
        QCOMPARE(bottomEdgeParent->childItems().last(), privateTestItem->styleItem);

        QQuickItem *dynamicItem = test->findItem<QQuickItem*>("dynamicItem");
        dynamicItem->setParentItem(bottomEdgeParent);
        // still the last one
        QCOMPARE(bottomEdgeParent->childItems().last(), privateTestItem->styleItem);
    }

    void test_commit_when_clicked()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        test->testItem()->hint()->setStatus(UCBottomEdgeHint::Locked);
        UCBottomEdgeHint *hint = test->testItem()->hint();
        QTest::mouseClick(test->testItem()->hint()->window(), Qt::LeftButton, 0, UbuntuTestCase::centerOf(hint, true).toPoint());
        QTRY_COMPARE_WITH_TIMEOUT(test->testItem()->status(), UCBottomEdge::Committed, 1000);
    }

    void test_commit_when_touch_clicked()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdgeHint *hint = test->testItem()->hint();
        // swipe a bit to reveal

        UCTestExtras::touchDrag(0, hint, QPoint(hint->width() / 2, hint->height()), QPoint(0, -hint->height()));
        QTRY_COMPARE_WITH_TIMEOUT(hint->status(), UCBottomEdgeHint::Active, 1000);

        UCTestExtras::touchClick(0, hint, UbuntuTestCase::centerOf(hint).toPoint());
        QTRY_COMPARE_WITH_TIMEOUT(test->testItem()->status(), UCBottomEdge::Committed, 1000);
    }

    void test_revealed_when_hint_threshold_passed_data()
    {
        QTest::addColumn<bool>("withMouse");
        QTest::addColumn<bool>("lockHint");
        QTest::addColumn<bool>("xfail");

        QTest::newRow("drag with mouse, unlocked hint") << true << false << true;
        QTest::newRow("drag with mouse, locked hint") << true << true << false;
        QTest::newRow("drag with touch, unlocked hint") << false << false << false;
        QTest::newRow("drag with touch, locked hint") << false << true << false;
    }
    void test_revealed_when_hint_threshold_passed()
    {
        QFETCH(bool, withMouse);
        QFETCH(bool, lockHint);
        QFETCH(bool, xfail);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgeStyle *style = UCBottomEdgePrivate::get(bottomEdge)->bottomPanel;
        QSignalSpy spy(bottomEdge, SIGNAL(statusChanged(UCBottomEdge::Status)));

        // swipe till we reveal it
        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 1);
        QPoint delta(0, -(2 * style->m_revealThreshold));
        if (lockHint) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
        }
        if (withMouse) {
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta);
        }
        if (xfail) {
            QEXPECT_FAIL(0, "failure expected", Continue);
        }
        QVERIFY(spy.wait(500));
        if (xfail) {
            QCOMPARE(spy.count(), 0);
        } else {
            // there must be two state changes here, one Hidden->Revealed, and one Revealed->Hidden
            QCOMPARE(spy.count(), 2);
            QList<QVariant> state1 = spy.at(0);
            QCOMPARE(state1[0].value<int>(), (int)UCBottomEdge::Revealed);
            QList<QVariant> state2 = spy.at(1);
            QCOMPARE(state2[0].value<int>(), (int)UCBottomEdge::Hidden);
        }
    }

    void test_commit_when_onethird_passed_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_commit_when_onethird_passed()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 1);
        QPoint delta(0, -(bottomEdge->height() / 3));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
        }
        if (withMouse) {
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta);
        }
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
    }

    void test_collapse_before_onethird()
    {

    }

    void test_collapse_when_dragged_downwards_data()
    {
        // when onethird not passed
        // when onethird was passed
    }
    void test_collapse_when_dragged_downwards()
    {

    }

    void test_height_less_than_parent()
    {

    }

    void test_collapse_during_commit()
    {

    }

    void test_commit_during_collapse()
    {

    }

    void test_alternative_content_for_default_commit_region()
    {

    }

    void test_page_as_content()
    {

    }

    void test_custom_content()
    {

    }

    void test_detect_page_header_in_content()
    {

    }

    void test_autocollapse_navigation_action_on_collapse_completed()
    {

    }

    void test_region_entered_triggered()
    {

    }

    void test_region_exited_triggered()
    {

    }

    void test_region_dragEnded_triggered()
    {

    }

    void test_region_from_and_to_data()
    {

    }
    void test_region_from_and_to()
    {

    }

    void test_overlapping_regions()
    {

    }

    void test_overlapping_regions_with_alternate_content()
    {

    }

    void test_custom_region_not_reaching_top()
    {

    }

    void test_drag_into_uncovered_region()
    {

    }

    void test_end_drag_in_region()
    {

    }

    void test_overload_region_drag_ending_with_commit()
    {

    }

    void test_commit_region_content()
    {

    }

    // this is a style hinted test, maybe move the feature to the public API
    void test_attach_hint_to_content()
    {

    }
};

QTEST_MAIN(tst_BottomEdge)

#include "tst_bottomedge.moc"
