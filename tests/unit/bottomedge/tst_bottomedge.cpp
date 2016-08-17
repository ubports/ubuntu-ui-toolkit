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
#include <UbuntuToolkit/private/ucbottomedge_p.h>
#include <UbuntuToolkit/private/ucbottomedgeregion_p.h>
#include "private/ucbottomedgeregion_p_p.h"
#include "private/ucbottomedge_p_p.h"
#include <UbuntuToolkit/private/ucbottomedgehint_p.h>
#include "private/ucswipearea_p_p.h"
#include <UbuntuToolkit/private/ucunits_p.h>
#include <UbuntuToolkit/private/ucheader_p.h>
#include <UbuntuToolkit/private/ucaction_p.h>
#include "uctestcase.h"
#include "uctestextras.h"
#define private public
#include <UbuntuToolkit/private/quickutils_p.h>
#include <UbuntuToolkit/private/ucbottomedgestyle_p.h>
#undef private

UT_USE_NAMESPACE

Q_DECLARE_METATYPE(Qt::Key)

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
        // make sure we disable the mouse
        QuickUtils::instance()->m_mouseAttached = false;

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

    bool hasContentAutoCollapse(const QString &testItem = "testItem")
    {
        UCBottomEdge *bottomEdge = this->testItem(testItem);
        QQuickItem *contentItem = bottomEdge->contentItem();
        if (!contentItem) {
            return false;
        }
        // we do not have the PageHeader in cpp, therefore we need the same workaround we have in the code
        UCHeader *header = contentItem->findChild<UCHeader*>();
        if (!QuickUtils::inherits(header, "PageHeader")) {
            return false;
        }

        QVariant list(header->property("navigationActions"));
        QQmlListProperty<UCAction> actions = list.value< QQmlListProperty<UCAction> >();
        QList<UCAction*> *navigationActions = reinterpret_cast<QList<UCAction*>*>(actions.data);
        if (navigationActions->size() <= 0) {
            return false;
        }
        return (qobject_cast<UCCollapseAction*>(navigationActions->at(0)) != Q_NULLPTR);
    }

    void guToPoints(QList<QPoint> &guMoves)
    {
        for (int i = 0; i < guMoves.size(); i++) {
            guMoves[i] = QPointF(UCUnits::instance()->gu(guMoves[i].x()), UCUnits::instance()->gu(guMoves[i].y())).toPoint();
        }
    }
};


class tst_BottomEdge : public QObject
{
    Q_OBJECT
    QStringList regionObjects;
private Q_SLOTS:

    void initTestCase()
    {
        UCTestExtras::registerTouchDevice();
    }

    void init()
    {
        regionObjects.clear();
    }

private Q_SLOTS:
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
        QCOMPARE(UCBottomEdgeRegionPrivate::get(test->regionAt("testItem", 0))->from, 0.0);
        QCOMPARE(UCBottomEdgeRegionPrivate::get(test->regionAt("testItem", 0))->to, 1.0);
        QVERIFY(!test->testItem()->activeRegion());
        QVERIFY(!test->testItem()->preloadContent());
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

    void test_overridden_triggers_bug1524234()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("OverriddenHintTrigger.qml"));
        test->testItem()->hint()->setStatus(UCBottomEdgeHint::Locked);
        UCBottomEdgeHint *hint = test->testItem()->hint();
        UCAction *action = hint->action();
        QSignalSpy actionSpy(action, SIGNAL(triggered(QVariant)));
        QSignalSpy hintSpy(hint, SIGNAL(triggered(QVariant)));

        QTest::mouseClick(test->testItem()->hint()->window(), Qt::LeftButton, 0, UbuntuTestCase::centerOf(hint, true).toPoint());
        QTRY_COMPARE_WITH_TIMEOUT(test->testItem()->status(), UCBottomEdge::Committed, 1000);

        QCOMPARE(actionSpy.count(), 0);
        QCOMPARE(hintSpy.count(), 1);
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
            UCTestExtras::touchDrag(0, bottomEdge, from, delta, 8);
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
            QCOMPARE(state1[0].value<UCBottomEdge::Status>(), UCBottomEdge::Revealed);
            QList<QVariant> state2 = spy.at(1);
            QCOMPARE(state2[0].value<UCBottomEdge::Status>(), UCBottomEdge::Hidden);
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
        // add some extra space for the touch
        QPoint delta(0, -(bottomEdge->height() / 3 + UCUnits::instance()->gu(6)));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
        }
        // we need to do the swipe in more steps
        if (withMouse) {
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton, 0, 20);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta, 20);
        }
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
    }

    void test_collapse_before_onethird_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_collapse_before_onethird()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 1);
        QPoint delta(0, -(bottomEdge->height() / 4));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
        }
        // we need to do the swipe in more steps
        if (withMouse) {
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton, 0, 20);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta, 20);
        }
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Hidden, 1000);
    }

    void test_collapse_when_dragged_downwards_data()
    {
        // when onethird not passed
        // when onethird was passed
        QTest::addColumn<bool>("withMouse");
        QTest::addColumn< QList<QPoint> >("moves");

        QList<QPoint> shortPath, longPath;
        // upwards
        for (int i = 0; i < 10; i++) {
            shortPath << QPoint(0, -3);
            longPath << QPoint(0, -7);
        }
        // downwards
        for (int i = 0; i < 5; i++) {
            shortPath << QPoint(0, 2);
            longPath << QPoint(0, 2);
        }

        QTest::newRow("with mouse, onethird not passed")
                << true << shortPath;
        QTest::newRow("with touch, onethird not passed")
                << false << shortPath;
        QTest::newRow("with mouse, onethird passed")
                << true << longPath;
        QTest::newRow("with touch, onethird passed")
                << false << longPath;
    }
    void test_collapse_when_dragged_downwards()
    {
        QFETCH(bool, withMouse);
        QFETCH(QList<QPoint>, moves);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        test->guToPoints(moves);

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 1);
        moves.prepend(from);

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
        }
        if (withMouse) {
            UCTestExtras::mouseDragWithPoints(bottomEdge, moves, Qt::LeftButton);
        } else {
            UCTestExtras::touchDragWithPoints(0, bottomEdge, moves);
        }
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Hidden, 1000);
    }

    void test_height_less_than_parent()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("ShorterBottomEdge.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgeStyle *style = UCBottomEdgePrivate::get(bottomEdge)->bottomPanel;
        bottomEdge->commit();
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
        QCOMPARE(style->m_panel->y(), bottomEdge->y());
    }

    void test_do_not_overshoot_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with touch") << false;
        QTest::newRow("with mouse") << true;
    }
    void test_do_not_overshoot()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("ShorterBottomEdge.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgeStyle *style = UCBottomEdgePrivate::get(bottomEdge)->bottomPanel;

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 1);
        QPoint to = from + QPoint(0, -(bottomEdge->parentItem()->height() - 1));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
            from = bottomEdge->mapToScene(from).toPoint();
            to = bottomEdge->mapToScene(to).toPoint();
            QTest::mousePress(bottomEdge->window(), Qt::LeftButton, 0, from, 20);
            QPoint movePos(from);
            while (movePos.y() > to.y()) {
                QTest::mouseMove(bottomEdge->window(), movePos, 20);
                movePos += QPoint(0, -10);
                QVERIFY(style->m_panel->y() >= bottomEdge->y());
            }
            QTest::mouseRelease(bottomEdge->window(),Qt::LeftButton, 0, movePos, 20);
        } else {
            UCTestExtras::touchPress(0, bottomEdge, from);
            QPoint movePos(from);
            while (movePos.y() > to.y()) {
                QTest::qWait(20);
                UCTestExtras::touchMove(0, bottomEdge, movePos);
                movePos += QPoint(0, -10);
                QVERIFY(style->m_panel->y() >= bottomEdge->y());
            }
            QTest::qWait(20);
            UCTestExtras::touchRelease(0, bottomEdge, movePos);
        }
    }

    void test_commitStarted_commitCompleted_emitted()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QSignalSpy commitStartedSpy(bottomEdge, SIGNAL(commitStarted()));
        QSignalSpy commitCompletedSpy(bottomEdge, SIGNAL(commitCompleted()));
        bottomEdge->commit();
        QTRY_COMPARE_WITH_TIMEOUT(commitStartedSpy.count(), 1, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(commitCompletedSpy.count(), 1, 1000);
    }

    void test_collapseStarted_collapseCompleted_emitted()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        bottomEdge->commit();
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
        // wait few milliseconds before we initiate collapse
        QTest::qWait(200);

        QSignalSpy collapseStartedSpy(bottomEdge, SIGNAL(collapseStarted()));
        QSignalSpy collapseCompletedSpy(bottomEdge, SIGNAL(collapseCompleted()));
        bottomEdge->collapse();
        QTRY_COMPARE_WITH_TIMEOUT(collapseStartedSpy.count(), 1, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(collapseCompletedSpy.count(), 1, 1000);
    }

    void test_collapse_during_commit()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QSignalSpy commitStartedSpy(bottomEdge, SIGNAL(commitStarted()));
        QSignalSpy commitCompletedSpy(bottomEdge, SIGNAL(commitCompleted()));
        QSignalSpy collapseStartedSpy(bottomEdge, SIGNAL(collapseStarted()));
        QSignalSpy collapseCompletedSpy(bottomEdge, SIGNAL(collapseCompleted()));

        connect(bottomEdge, SIGNAL(commitStarted()), bottomEdge, SLOT(collapse()), Qt::QueuedConnection);

        bottomEdge->commit();

        QTRY_COMPARE_WITH_TIMEOUT(commitStartedSpy.count(), 1, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(commitCompletedSpy.count(), 0, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(collapseStartedSpy.count(), 1, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(collapseCompletedSpy.count(), 1, 1000);
    }

    void test_commit_during_collapse()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        bottomEdge->commit();

        QSignalSpy commitStartedSpy(bottomEdge, SIGNAL(commitStarted()));
        QSignalSpy commitCompletedSpy(bottomEdge, SIGNAL(commitCompleted()));
        QSignalSpy collapseStartedSpy(bottomEdge, SIGNAL(collapseStarted()));
        QSignalSpy collapseCompletedSpy(bottomEdge, SIGNAL(collapseCompleted()));

        connect(bottomEdge, SIGNAL(collapseStarted()), bottomEdge, SLOT(commit()), Qt::QueuedConnection);

        bottomEdge->collapse();
        QTRY_COMPARE_WITH_TIMEOUT(collapseStartedSpy.count(), 1, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(collapseCompletedSpy.count(), 0, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(commitStartedSpy.count(), 1, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(commitCompletedSpy.count(), 1, 1000);
    }

    // region tests

    void test_region_operations_data()
    {
        QTest::addColumn<QString>("document");
        QTest::addColumn<QString>("warning");
        QTest::addColumn<qreal>("xFrom");
        QTest::addColumn<qreal>("xTo");
        QTest::addColumn<QString>("xName");

        QTest::newRow("add through regions property")
                << "AddCustomRegionUsingRegionsProperty.qml"
                << QString()
                << 0.0 << 1.0 << QString("customRegion");
        QTest::newRow("add through data property")
                << "AddCustomRegionUsingDataProperty.qml"
                << QString()
                << 0.0 << 1.0 << QString("customRegion");
        QTest::newRow("add through Component.onCompleted")
                << "AddCustomRegionOnCompleted.qml"
                << QString()
                << 0.0 << 1.0 << QString("customRegion");
        QTest::newRow("add owned by other BottomEdge")
                << "AddCustomRegionOwnedByOtherBottomEdge.qml"
                << "QML BottomEdge: Cannot reuse region owned by other BottomEdge components"
                   // we should have the default region still
                << 0.0 << 1.0 << "default_BottomEdgeRegion";
        QTest::newRow("clear")
                << "ClearCustomRegions.qml"
                << QString()
                   // we should have the default region back
                << 0.0 << 1.0 << "default_BottomEdgeRegion";
    }

    void test_region_operations()
    {
        QFETCH(QString, document);
        QFETCH(QString, warning);
        QFETCH(qreal, xFrom);
        QFETCH(qreal, xTo);
        QFETCH(QString, xName);

        if (!warning.isEmpty()) {
            UbuntuTestCase::ignoreWarning(document, 26, 5, warning, 1);
        }
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase(document));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);

        // the regions has the custom one
        QCOMPARE(privateBottomEdge->regions.size(), 1);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];
        QVERIFY(region);
        QCOMPARE(UCBottomEdgeRegionPrivate::get(region)->from, xFrom);
        QCOMPARE(UCBottomEdgeRegionPrivate::get(region)->to, xTo);
        QCOMPARE(region->objectName(), xName);
    }

    void test_active_region_changes()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("LeanActiveRegionChange.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint to = from + QPoint(0, -(bottomEdge->parentItem()->height() - 1));
        QSignalSpy spy(bottomEdge, SIGNAL(activeRegionChanged(UCBottomEdgeRegion*)));

        UCTestExtras::touchPress(0, bottomEdge, from);
        QPoint movePos(from);
        while (movePos.y() > to.y()) {
            QTest::qWait(20);
            UCTestExtras::touchMove(0, bottomEdge, movePos);
            movePos += QPoint(0, -10);
        }
        QTest::qWait(20);
        UCTestExtras::touchRelease(0, bottomEdge, movePos);
        // we should have had 4 active region changes by now
        // null -> defaultRegion -> region #0 -> region #1 -> defaultRegion
        QCOMPARE(spy.count(), 4);
    }

    void test_region_signals_emitted_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_region_signals_emitted()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];

        // change the region so we can get the signals while dragged
        UCBottomEdgeRegionPrivate::get(region)->from = 0.1;
        UCBottomEdgeRegionPrivate::get(region)->to = 0.2;

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint delta(0, -(bottomEdge->height() / 3 + UCUnits::instance()->gu(6)));
        QSignalSpy entered(region, SIGNAL(entered()));
        QSignalSpy exited(region, SIGNAL(exited()));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
        }
        if (withMouse) {
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton, 0);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta);
        }
        QTRY_COMPARE_WITH_TIMEOUT(entered.count(), 1, 500);
        // exit should not be emitted as the region is not left when committed
        QTRY_COMPARE_WITH_TIMEOUT(exited.count(), 0, 500);
    }

    void test_region_dragEnded_emitted_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_region_dragEnded_emitted()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeInItem.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];

        QSignalSpy dragEnded(region, SIGNAL(dragEnded()));
        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint delta(0, -(bottomEdge->height() / 2.0f));
        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton, 0);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta);
        }
        UbuntuTestCase::waitForSignal(&dragEnded);
    }

    void test_alternative_content_for_default_commit_region()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("AlternateDefaultRegionContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint delta(0, -(bottomEdge->height() / 2));

        UCTestExtras::touchDrag(0, bottomEdge, from, delta, 20);
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
        QCOMPARE(bottomEdge->contentItem()->objectName(), QString("regionContent"));
    }

    void test_end_drag_in_region_commits_to_the_region_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_end_drag_in_region_commits_to_the_region()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("AlternateDefaultRegionContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];
        UCBottomEdgeStyle *style = UCBottomEdgePrivate::get(bottomEdge)->bottomPanel;

        // alter default region for testing
        UCBottomEdgeRegionPrivate::get(region)->to = 0.6;

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint delta(0, -(bottomEdge->height() / 2));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton, 0);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta, 20);
        }

        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
        QCOMPARE(bottomEdge->activeRegion(), region);
        // the top of the committed content should not be the top of the bottom edge
        QVERIFY(style->m_panel->y() > bottomEdge->y());
    }

    void test_drag_ends_in_uncovered_region_collapses_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_drag_ends_in_uncovered_region_collapses()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("UncoveredByRegion.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 1);
        QPoint to = from + QPoint(0, -(bottomEdge->parentItem()->height() - UCUnits::instance()->gu(10)));
        // let us know when we are out of the region
        QSignalSpy exitRegion(region, SIGNAL(exited()));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
            from = bottomEdge->mapToScene(from).toPoint();
            to = bottomEdge->mapToScene(to).toPoint();
            QTest::mousePress(bottomEdge->window(), Qt::LeftButton, 0, from, 20);
            QPoint movePos(from);
            while (movePos.y() > to.y()) {
                QTest::mouseMove(bottomEdge->window(), movePos, 20);
                movePos += QPoint(0, -20);
            }
            movePos -= QPoint(0, -20);
            QTest::mouseRelease(bottomEdge->window(),Qt::LeftButton, 0, movePos, 20);
        } else {
            UCTestExtras::touchPress(0, bottomEdge, from);
            QPoint movePos(from);
            while (movePos.y() > to.y()) {
                QTest::qWait(20);
                UCTestExtras::touchMove(0, bottomEdge, movePos);
                movePos += QPoint(0, -20);
            }
            movePos -= QPoint(0, -20);
            QTest::qWait(20);
            UCTestExtras::touchRelease(0, bottomEdge, movePos);
        }

        // we should be collapsing!
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
    }

    void test_commit_region_content_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_commit_region_content()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("AlternateRegionContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];

        // alter region for testing
        UCBottomEdgeRegionPrivate::get(region)->from = 0.1;
        UCBottomEdgeRegionPrivate::get(region)->to = 0.8;
        // and connect commit to dragEnded
        connect(region, &UCBottomEdgeRegion::dragEnded, bottomEdge, &UCBottomEdge::commit);

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint to = from + QPoint(0, -(bottomEdge->parentItem()->height() - 1));

        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
            from = bottomEdge->mapToScene(from).toPoint();
            to = bottomEdge->mapToScene(to).toPoint();
            QTest::mousePress(bottomEdge->window(), Qt::LeftButton, 0, from, 20);
            QPoint movePos(from);
            while (movePos.y() > to.y() && (bottomEdge->activeRegion() != region)) {
                QTest::mouseMove(bottomEdge->window(), movePos, 20);
                movePos += QPoint(0, -10);
            }
            QTest::mouseRelease(bottomEdge->window(),Qt::LeftButton, 0, movePos, 20);
        } else {
            UCTestExtras::touchPress(0, bottomEdge, from);
            QPoint movePos(from);
            while (movePos.y() > to.y() && (bottomEdge->activeRegion() != region)) {
                QTest::qWait(20);
                UCTestExtras::touchMove(0, bottomEdge, movePos);
                movePos += QPoint(0, -10);
            }
            QTest::qWait(20);
            UCTestExtras::touchRelease(0, bottomEdge, movePos);
        }
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 1000);
        QCOMPARE(bottomEdge->contentItem()->objectName(), QString("regionContent"));
    }

    void test_overlapping_regions()
    {
        QString document("OverlappingRegions.qml");
        UbuntuTestCase::ignoreWarning(document, 34, 9, "QML BottomEdgeRegion: Region intersects the one from index 0 having from: 0.2 and to: 0.5", 1);
        UbuntuTestCase::ignoreWarning(document, 37, 9, "QML BottomEdgeRegion: Region intersects the one from index 0 having from: 0.2 and to: 0.5", 1);
        UbuntuTestCase::ignoreWarning(document, 37, 9, "QML BottomEdgeRegion: Region at index 1 contains this region. This region will never activate.", 1);
        UbuntuTestCase::ignoreWarning(document, 41, 9, "QML BottomEdgeRegion: Region at index 1 contains this region. This region will never activate.", 1);
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase(document));
    }

    void test_region_does_not_activate_when_from_greater_than_to_data()
    {
        QTest::addColumn<bool>("withMouse");

        QTest::newRow("with mouse") << true;
        QTest::newRow("with touch") << false;
    }
    void test_region_does_not_activate_when_from_greater_than_to()
    {
        QFETCH(bool, withMouse);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("AlternateRegionContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCBottomEdgePrivate *privateBottomEdge = UCBottomEdgePrivate::get(bottomEdge);
        UCBottomEdgeRegion *region = privateBottomEdge->regions[0];

        // adjust region data for the test
        UCBottomEdgeRegionPrivate::get(region)->from = 0.4;
        UCBottomEdgeRegionPrivate::get(region)->to = 0.2;
        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint delta(0, -(bottomEdge->height() / 2.0f));
        QSignalSpy activeRegion(bottomEdge, SIGNAL(activeRegionChanged(UCBottomEdgeRegion*)));
        if (withMouse) {
            bottomEdge->hint()->setStatus(UCBottomEdgeHint::Locked);
            UCTestExtras::mouseDrag(bottomEdge, from, delta, Qt::LeftButton, 0, 10);
        } else {
            UCTestExtras::touchDrag(0, bottomEdge, from, delta, 10);
        }
        QEXPECT_FAIL(0, "region should not activate", Continue);
        QVERIFY(activeRegion.wait(400));
    }

    void test_autocollapse_navigation_action_on_commit_completed_data()
    {
        QTest::addColumn<QString>("document");

        QTest::newRow("content has PageHeader") << "AutoCollapseInPageHeader.qml";
        QTest::newRow("content is Page with PageHeader") << "AutoCollapseInPageWithPageHeader.qml";
    }
    void test_autocollapse_navigation_action_on_commit_completed()
    {
        QFETCH(QString, document);

        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase(document));
        UCBottomEdge *bottomEdge = test->testItem();

        BottomEdgeTestCase *testCase = test.data();
        connect(bottomEdge, &UCBottomEdge::contentItemChanged, [=]() {
            QVERIFY(!testCase->hasContentAutoCollapse());
        });
        connect(bottomEdge, &UCBottomEdge::commitCompleted, [=]() {
            QVERIFY(testCase->hasContentAutoCollapse());
        });
        // drag slowly
        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint delta(0, -bottomEdge->height());
        UCTestExtras::touchDrag(0, bottomEdge, from, delta, 20);
    }

    void test_bottomedge_hint_enabled() {
        QScopedPointer<BottomEdgeTestCase> view(new BottomEdgeTestCase("Defaults.qml"));
        UCBottomEdge *bottomEdge = view->testItem();
        QCOMPARE(bottomEdge->isEnabled(), bottomEdge->hint()->isEnabled());
        bottomEdge->setEnabled(!bottomEdge->isEnabled());
        QCOMPARE(bottomEdge->isEnabled(), bottomEdge->hint()->isEnabled());
    }

    void test_collapse_by_keyboard_data()
    {
        QTest::addColumn<Qt::Key>("key");

        QTest::newRow("space") << Qt::Key_Space;
        QTest::newRow("enter") << Qt::Key_Enter;
        QTest::newRow("return") << Qt::Key_Return;
    }
    void test_collapse_by_keyboard() {
        QFETCH(Qt::Key, key);

        QScopedPointer<BottomEdgeTestCase> view(new BottomEdgeTestCase("Defaults.qml"));
        view->rootObject()->forceActiveFocus();
        QTRY_COMPARE_WITH_TIMEOUT(view->rootObject()->property("activeFocus").toBool(), true, 1000);
        UCBottomEdge *bottomEdge = view->testItem();
        QTest::keyClick(bottomEdge->hint()->window(), Qt::Key_Tab);
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->hint()->property("activeFocus").toBool(), true, 1000);
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->hint()->property("keyNavigationFocus").toBool(), true, 1000);
        QTest::keyClick(bottomEdge->hint()->window(), key);
        QSignalSpy commitCompletedSpy(bottomEdge, SIGNAL(commitCompleted()));
        QTRY_COMPARE_WITH_TIMEOUT(commitCompletedSpy.count(), 1, 1000);
        QTest::keyClick(bottomEdge->hint()->window(), Qt::Key_Escape);
        QSignalSpy collapseCompletedSpy(bottomEdge, SIGNAL(collapseCompleted()));
        QTRY_COMPARE_WITH_TIMEOUT(collapseCompletedSpy.count(), 1, 1000);
    }

    void test_preload_content()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("PreloadedContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        QPoint from(bottomEdge->width() / 2.0f, bottomEdge->height() - 5);
        QPoint to = from + QPoint(0, -(bottomEdge->parentItem()->height() - 1));
        QSignalSpy spy(bottomEdge, SIGNAL(activeRegionChanged(UCBottomEdgeRegion*)));

        connect(bottomEdge, &UCBottomEdge::contentItemChanged, [=]() {
            regionObjects.append(bottomEdge->contentItem()
                                    ? bottomEdge->contentItem()->objectName()
                                    : "NULL");
        });

        UCTestExtras::touchPress(0, bottomEdge, from);
        QPoint movePos(from);
        while (movePos.y() > to.y()) {
            QTest::qWait(20);
            UCTestExtras::touchMove(0, bottomEdge, movePos);
            movePos += QPoint(0, -10);
        }
        QTest::qWait(20);
        UCTestExtras::touchRelease(0, bottomEdge, movePos);
        // we should have had 4 active region changes by now
        // null -> defaultRegion -> region #0 -> region #1 -> null
        QCOMPARE(spy.count(), 4);
        QCOMPARE(regionObjects.size(), 6);
        // the first two regions content is the same
        int i = 0;
        QCOMPARE(regionObjects[i++], QString("default"));
        QCOMPARE(regionObjects[i++], QString("default"));
        QCOMPARE(regionObjects[i++], QString("region1"));
        QCOMPARE(regionObjects[i++], QString("default"));
        QCOMPARE(regionObjects[i++], QString("region2"));
        QCOMPARE(regionObjects[i++], QString("default"));
    }

    void test_preload_content_url()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("PreloadContentUrl.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        // commit so the contentItem is set
        bottomEdge->commit();
        QTRY_VERIFY(bottomEdge->contentItem());
    }

    void test_reset_preload_content()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("PreloadedContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        UCBottomEdgePrivate *d = UCBottomEdgePrivate::get(bottomEdge);
        for (int i = 0; i < d->regions.size(); i++) {
            QVERIFY(UCBottomEdgeRegionPrivate::get(d->regions[i])->contentItem);
        }

        // set preloadContent: false
        bottomEdge->setPreloadContent(false);
        for (int i = 0; i < d->regions.size(); i++) {
            QVERIFY(!UCBottomEdgeRegionPrivate::get(d->regions[i])->contentItem);
        }
    }

    void test_disabled_content_unloads()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("PreloadedContent.qml"));
        UCBottomEdge *bottomEdge = test->testItem();

        UCBottomEdgePrivate *d = UCBottomEdgePrivate::get(bottomEdge);
        // disable a region
        d->regions[0]->setEnabled(false);
        QVERIFY(!UCBottomEdgeRegionPrivate::get(d->regions[0])->contentItem);

        // enable it
        d->regions[0]->setEnabled(true);
        QTRY_VERIFY_WITH_TIMEOUT(UCBottomEdgeRegionPrivate::get(d->regions[0])->contentItem != nullptr, 1000);
    }

    void test_action_triggered_commits()
    {
        QScopedPointer<BottomEdgeTestCase> test(new BottomEdgeTestCase("BottomEdgeWithAction.qml"));
        UCBottomEdge *bottomEdge = test->testItem();
        UCAction *action = bottomEdge->hint()->action();
        QVERIFY(action);

        // trigger action
        action->trigger();
        QTRY_COMPARE_WITH_TIMEOUT(bottomEdge->status(), UCBottomEdge::Committed, 2000);
        QVERIFY(bottomEdge->contentItem());
    }
};

QTEST_MAIN(tst_BottomEdge)

#include "tst_bottomedge.moc"
