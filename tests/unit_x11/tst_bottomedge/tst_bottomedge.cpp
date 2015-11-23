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
#include "uctestcase.h"
#include "uctestextras.h"

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
    {}

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
    }

    void cleanup()
    {

    }

    void test_defaults()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("Defaults.qml"));
        QCOMPARE(test->testItem()->height(), test->rootObject()->height());
        QCOMPARE(test->testItem()->dragProgress(), 0.0);
        QCOMPARE(test->testItem()->dragDirection(), UCBottomEdge::Undefined);
        QCOMPARE(test->testItem()->state(), UCBottomEdge::Hidden);
        QCOMPARE(test->testItem()->content(), QUrl());
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

    void test_last_item_of_parent()
    {

    }

    void test_commit_when_clicked()
    {

    }

    void test_revealed_when_hint_threshold_passed()
    {

    }

    void test_commit_when_onethird_passed()
    {

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
