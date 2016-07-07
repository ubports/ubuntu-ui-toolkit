/*
 * Copyright 2015-2016 Canonical Ltd.
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
#include <QtCore/QObject>
#include <QtQuick/QQuickView>
#include <QtQml/QQmlEngine>
#include <QPointer>
#include <private/qquickmousearea_p.h>
#include <private/qquickwindow_p.h>

#include "private/ucswipearea_p.h"
#include "private/ucswipearea_p_p.h"
#define protected public
#define private public
#include <UbuntuGestures/TouchRegistry>
#undef protected
#undef private

#include "GestureTest.h"
#include "uctestcase.h"

using namespace UbuntuGestures;

// Because QSignalSpy(UCSwipeArea, SIGNAL(UCSwipeArea::Status)) simply
// doesn't work
class StatusSpy : public UCSwipeAreaStatusListener
{
public:
    StatusSpy(UCSwipeArea *edgeDragArea)
        : m_edgeDragArea(edgeDragArea)
        , m_recognized(false)
    {
        UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(m_edgeDragArea);
        d->addStatusChangeListener(this);
    }
    virtual ~StatusSpy()
    {
        UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(m_edgeDragArea);
        d->removeStatusChangeListener(this);
    }

    bool recognized() {
        return m_recognized;
    }

private:
    void swipeStatusChanged(UCSwipeAreaPrivate::Status, UCSwipeAreaPrivate::Status status) override
    {
        m_recognized |= status == UCSwipeAreaPrivate::Recognized;
    }

private:
    UCSwipeArea *m_edgeDragArea;
    bool m_recognized;
};

/*
    QQuickMouseArea::canceled() signal is not registered in the meta object system.
    So using a QSignalSpy to track it won't work. Thus the only way to connect to it
    is using its method address directly.
 */
class MouseAreaSpy : public QObject
{
    Q_OBJECT
public:
    MouseAreaSpy(QQuickMouseArea *mouseArea)
        : canceledCount(0)
    {
        connect(mouseArea, &QQuickMouseArea::canceled,
                this, &MouseAreaSpy::onMouseAreaCanceled);
    }

    int canceledCount;

private Q_SLOTS:
    void onMouseAreaCanceled() {
        ++canceledCount;
    }
};

class tst_UCSwipeArea: public GestureTest
{
    Q_OBJECT
public:
    tst_UCSwipeArea();
private Q_SLOTS:
    void init() override; // called right before each and every test function is executed

    void dragWithShortDirectionChange();
    void recognitionTimerUsage();
    void sceneXAndX();
    void sceneYAndY();
    void twoFingerTap();
    void movingDDA();
    void ignoreOldFinger();
    void rotated();
    void distance();
    void distance_data();
    void disabledWhileDragging();
    void oneFingerDownFollowedByLateSecondFingerDown();
    void givesUpWhenLosesTouch();
    void threeFingerDrag();
    void immediateRecognitionWhenConstraintsDisabled();
    void withdrawTouchOwnershipCandidacyIfDisabledDuringRecognition();
    void withdrawTouchOwnershipCandidacyIfDisabledDuringRecognition_data();
    void gettingTouchOwnershipMakesMouseAreaBehindGetCanceled();
    void interleavedTouches();
    void makoRightEdgeDrag();
    void makoRightEdgeDrag_verticalDownwards();
    void makoLeftEdgeDrag_slowStart();
    void makoLeftEdgeDrag_movesSlightlyBackwardsOnStart();
    void grabGesture();
    void grabGestureWithImmediateRecognition();

private:
    // QTest::touchEvent takes QPoint instead of QPointF and I don't want to
    // lose precision due to rounding.
    // Besides, those helper functions lead to more compact code.
    void sendTouchPress(qint64 timestamp, int id, QPointF pos);
    void sendTouchUpdate(qint64 timestamp, int id, QPointF pos);
    void sendTouchRelease(qint64 timestamp, int id, QPointF pos);
    void sendTouch(qint64 timestamp, int id, QPointF pos,
            Qt::TouchPointState pointState, QEvent::Type eventType);

    void passTime(qint64 timeSpanMs);
};

tst_UCSwipeArea::tst_UCSwipeArea()
    : GestureTest(QStringLiteral("tst_swipearea.qml"))
{
}

void tst_UCSwipeArea::init()
{
    GestureTest::init();

    // We shouldn't need the three lines below, but a compiz/unity7
    // regression means we don't pass the test without them because
    // the window doesn't have the proper size. Can be removed in the
    // future if the regression is fixed and tests pass again
    m_view->resize(m_view->rootObject()->width(), m_view->rootObject()->height());
    QTRY_COMPARE(m_view->width(), (int)m_view->rootObject()->width());
    QTRY_COMPARE(m_view->height(), (int)m_view->rootObject()->height());
}

void tst_UCSwipeArea::sendTouchPress(qint64 timestamp, int id, QPointF pos)
{
    sendTouch(timestamp, id, pos, Qt::TouchPointPressed, QEvent::TouchBegin);
}

void tst_UCSwipeArea::sendTouchUpdate(qint64 timestamp, int id, QPointF pos)
{
    sendTouch(timestamp, id, pos, Qt::TouchPointMoved, QEvent::TouchUpdate);
}

void tst_UCSwipeArea::sendTouchRelease(qint64 timestamp, int id, QPointF pos)
{
    sendTouch(timestamp, id, pos, Qt::TouchPointReleased, QEvent::TouchEnd);
}

void tst_UCSwipeArea::sendTouch(qint64 timestamp, int id, QPointF pos,
                                 Qt::TouchPointState pointState, QEvent::Type eventType)
{
    m_fakeTimerFactory->updateTime(timestamp);

    QTouchEvent::TouchPoint point;

    point.setState(pointState);
    point.setId(id);
    point.setScenePos(pos);
    point.setPos(pos);

    QList<QTouchEvent::TouchPoint> points;
    points << point;

    QTouchEvent touchEvent(eventType, m_device, Qt::NoModifier, Qt::TouchPointPressed, points);
    QCoreApplication::sendEvent(m_view, &touchEvent);

    QQuickWindowPrivate *windowPrivate = QQuickWindowPrivate::get(m_view);
    windowPrivate->flushDelayedTouchEvent();
}

void tst_UCSwipeArea::passTime(qint64 timeSpanMs)
{
    qint64 finalTime = m_fakeTimerFactory->timeSource()->msecsSinceReference() + timeSpanMs;
    m_fakeTimerFactory->updateTime(finalTime);
}

namespace {
QPointF calculateInitialtouchPosition(UCSwipeArea *edgeDragArea)
{
    QPointF localCenter(edgeDragArea->width() / 2., edgeDragArea->height() / 2.);
    return edgeDragArea->mapToScene(localCenter);
}
}

/*
  A directional drag should still be recognized if there is a momentaneous, small,
  change in the direction of a drag. That should be accounted as input noise and
  therefore ignored.
 */
void tst_UCSwipeArea::dragWithShortDirectionChange()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF initialtouchPosition = calculateInitialtouchPosition(edgeDragArea);
    QPointF touchPoint = initialtouchPosition;

    qreal desiredDragDistance = d->distanceThreshold * 2.0;
    QPointF dragDirectionVector(1.0, 0.0);
    qreal touchStepDistance = d->distanceThreshold * 0.1f;
    // make sure we are above maximum time
    int touchStepTimeMs = d->maxTime / 20. ;
    QPointF touchMovement = dragDirectionVector * touchStepDistance;

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    // Move a bit in the proper direction
    for (int i=0; i < 3; ++i) {
        touchPoint += touchMovement;
        passTime(touchStepTimeMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    // Then a sudden and small movement to the opposite direction
    touchPoint -= touchMovement*0.2;
    passTime(touchStepTimeMs*0.2);
    QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);

    // And then resume movment in the correct direction until it crosses the distance and time
    // thresholds.
    do {
        touchPoint += touchMovement;
        passTime(touchStepTimeMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    } while ((touchPoint - initialtouchPosition).manhattanLength() < desiredDragDistance
            || m_fakeTimerFactory->timeSource()->msecsSinceReference() < (d->compositionTime * 1.5f));

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    QTest::touchEvent(m_view, m_device).release(0, touchPoint.toPoint(), (QWindow*)nullptr);
}

/*
    Checks that the recognition timer is started and stopped appropriately.
    I.e., check that it's running only while gesture recognition is taking place
    (status == Undecided)
 */
void tst_UCSwipeArea::recognitionTimerUsage()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != 0);
    AbstractTimer *fakeTimer = m_fakeTimerFactory->createTimer();
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(fakeTimer);
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    int timeStepMs = 5; // some arbitrary small value.

    QPointF initialtouchPosition = calculateInitialtouchPosition(edgeDragArea);
    QPointF touchPoint = initialtouchPosition;

    QPointF dragDirectionVector(1.0, 0.0);
    QPointF touchMovement = dragDirectionVector * (d->distanceThreshold * 0.2f);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);
    QVERIFY(!fakeTimer->isRunning());

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);
    QVERIFY(fakeTimer->isRunning());

    // Move beyond distance threshold and composition time to ensure recognition
    while (m_fakeTimerFactory->timeSource()->msecsSinceReference() <= d->compositionTime ||
           (touchPoint - initialtouchPosition).manhattanLength() <= d->distanceThreshold) {

        QCOMPARE(d->status == UCSwipeAreaPrivate::Undecided, fakeTimer->isRunning());

        touchPoint += touchMovement;
        passTime(timeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);
    QVERIFY(!fakeTimer->isRunning());
}

/*
  Checks that it informs the X coordinate of the touch point in local and scene coordinates
  correctly.
 */
void tst_UCSwipeArea::sceneXAndX()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hnDragArea");
    QVERIFY(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());
    edgeDragArea->setImmediateRecognition(true);

    QPointF touchScenePosition(m_view->width() - (edgeDragArea->width()/2.0f), m_view->height()/2.0f);

    sendTouchPress(0 /* timestamp */, 0 /* id */, touchScenePosition);

    QSignalSpy touchSpy(edgeDragArea, &UCSwipeArea::touchPositionChanged);

    touchScenePosition.rx() = m_view->width() / 2;
    sendTouchUpdate(50 /* timestamp */, 0 /* id */, touchScenePosition);

    QCOMPARE(touchSpy.count(), 1);
    QCOMPARE(edgeDragArea->touchPosition().x(), touchScenePosition.x() - edgeDragArea->x());
}

/*
  Checks that it informs the Y coordinate of the touch point in local and scene coordinates
  correctly.
 */
void tst_UCSwipeArea::sceneYAndY()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("vnDragArea");
    QVERIFY(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());
    edgeDragArea->setImmediateRecognition(true);

    QPointF touchScenePosition(m_view->width()/2.0f, m_view->height() - (edgeDragArea->height()/2.0f));

    sendTouchPress(0 /* timestamp */, 0 /* id */, touchScenePosition);

    QSignalSpy touchSpy(edgeDragArea, &UCSwipeArea::touchPositionChanged);

    touchScenePosition.ry() = m_view->height() / 2;
    sendTouchUpdate(50 /* timestamp */, 0 /* id */, touchScenePosition);

    QCOMPARE(touchSpy.count(), 1);
    QCOMPARE(edgeDragArea->touchPosition().y(), touchScenePosition.y() - edgeDragArea->y());
}

/*
  Regression test for https://bugs.launchpad.net/bugs/1228336

  Perform a quick two-finger double-tap and check that UCSwipeArea properly
  rejects those touch points. In the bug above it got confused and crashed.
 */
void tst_UCSwipeArea::twoFingerTap()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // Make touches evenly spaced along the edgeDragArea
    QPoint touchAPos(edgeDragArea->width()/2.0f, m_view->height()*0.33f);
    QPoint touchBPos(edgeDragArea->width()/2.0f, m_view->height()*0.66f);

    qint64 timeStepMsecs = 5; // some arbitrary, small value

    // Perform the first two-finger tap
    // NB: using move() instead of stationary() becasue in the latter you cannot provide
    //     the touch position and therefore it's left with some garbage numbers.
    QTest::touchEvent(m_view, m_device)
        .press(0, touchAPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .move(0, touchAPos, (QWindow*)nullptr)
        .press(1, touchBPos, (QWindow*)nullptr);

    // A second touch point appeared during recognition, reject immediately as this
    // can't be a single-touch gesture anymore.
    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .release(0, touchAPos, (QWindow*)nullptr)
        .move(1, touchBPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .release(1, touchBPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    // Perform the second two-finger tap

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .press(0, touchAPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .move(0, touchAPos, (QWindow*)nullptr)
        .press(1, touchBPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .release(0, touchAPos, (QWindow*)nullptr)
        .move(1, touchBPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(timeStepMsecs);
    QTest::touchEvent(m_view, m_device)
        .release(1, touchBPos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);
}

/*
   Tests that gesture recognition works normally even if the UCSwipeArea moves
   during recognition.
   This effectively means that we have to do gesture recognition with scene coordinates
   instead of local item coordinates.
 */
void tst_UCSwipeArea::movingDDA()
{
    QQuickItem *rightwardsLauncher =  m_view->rootObject()->findChild<QQuickItem*>("rightwardsLauncher");
    Q_ASSERT(rightwardsLauncher != 0);

    UCSwipeArea *edgeDragArea =
        rightwardsLauncher->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF initialtouchPosition = calculateInitialtouchPosition(edgeDragArea);
    QPointF touchPoint = initialtouchPosition;

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;
    QPointF dragDirectionVector(1.0f, 0.0f);

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    // Move it far ahead along the direction of the gesture
    // rightwardsLauncher is a parent of our UCSwipeArea. So moving it will move our DDA
    rightwardsLauncher->setX(rightwardsLauncher->x() + d->distanceThreshold * 5.0f);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    QTest::touchEvent(m_view, m_device).release(0, touchPoint.toPoint(), (QWindow*)nullptr);
}

/*
    The presence of and old, rejected, touch point lying on the UCSwipeArea
    shouldn't impede the recognition of a gesture from a new touch point.
 */
void tst_UCSwipeArea::ignoreOldFinger()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // Make touches evenly spaced along the edgeDragArea
    QPoint touch0Pos(edgeDragArea->width()/2.0f, m_view->height()*0.33f);
    QPoint touch1Pos(edgeDragArea->width()/2.0f, m_view->height()*0.66f);

    QTest::touchEvent(m_view, m_device).press(0, touch0Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    // leave it lying around for some time
    passTime(d->maxTime * 10);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;
    QPointF dragDirectionVector(1.0f, 0.0f);

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .press(1, touch1Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touch1Pos += touchMovement.toPoint();
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device)
            .move(0, touch0Pos, (QWindow*)nullptr)
            .move(1, touch1Pos, (QWindow*)nullptr);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .release(1, touch1Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);
}

/*
    A Rightwards DDA that is rotated 90 degrees clockwise should recognize gestures
    that are done downwards in scene coordinates. I.e. the gesture recognition direction
    should be in local coordinates, not scene coordinates.
 */
void tst_UCSwipeArea::rotated()
{
    QQuickItem *baseItem =  m_view->rootObject()->findChild<QQuickItem*>("baseItem");
    baseItem->setRotation(90.);

    QQuickItem *rightwardsLauncher =  m_view->rootObject()->findChild<QQuickItem*>("rightwardsLauncher");
    Q_ASSERT(rightwardsLauncher != 0);

    UCSwipeArea *edgeDragArea =
        rightwardsLauncher->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF initialtouchPosition = calculateInitialtouchPosition(edgeDragArea);
    QPointF touchPoint = initialtouchPosition;

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;
    QPointF dragDirectionVector(0.0f, 1.0f);

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    QTest::touchEvent(m_view, m_device).release(0, touchPoint.toPoint(), (QWindow*)nullptr);
}

void tst_UCSwipeArea::distance()
{
    QQuickItem *baseItem =  m_view->rootObject()->findChild<QQuickItem*>("baseItem");
    QFETCH(qreal, rotation);
    QFETCH(QPointF, dragDirectionVector);
    baseItem->setRotation(rotation);

    QQuickItem *rightwardsLauncher =  m_view->rootObject()->findChild<QQuickItem*>("rightwardsLauncher");
    Q_ASSERT(rightwardsLauncher != 0);

    UCSwipeArea *edgeDragArea =
        rightwardsLauncher->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // to disable the position smoothing so that we can more easily check distance values
    edgeDragArea->setImmediateRecognition(true);

    QPointF initialtouchPosition = calculateInitialtouchPosition(edgeDragArea);
    QPointF touchPoint = initialtouchPosition;

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    qint64 timestamp = 0;

    sendTouchPress(timestamp, 0, touchPoint);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        timestamp += movementTimeStepMs;
        sendTouchUpdate(timestamp, 0, touchPoint);
    }

    qreal actualDragDistance = ((qreal)totalMovementSteps) * movementStepDistance;

    // UCSwipeArea::distance() must match the actual drag distance as the
    // drag was aligned with the gesture direction
    // NB: qFuzzyCompare(), used internally by QCOMPARE(), is broken.
    QVERIFY(qAbs(edgeDragArea->distance() - actualDragDistance) < 0.001);

    timestamp += movementTimeStepMs;
    sendTouchRelease(timestamp, 0, touchPoint);
}

void tst_UCSwipeArea::distance_data()
{
    QTest::addColumn<qreal>("rotation");
    QTest::addColumn<QPointF>("dragDirectionVector");

    QTest::newRow("not rotated")           << 0.  << QPointF(1., 0.);
    QTest::newRow("rotated by 90 degrees") << 90. << QPointF(0., 1.);
}

/*
    Regression test for https://bugs.launchpad.net/unity8/+bug/1276122

    The bug:
    If setting "enabled: false" while the UCSwipeArea's (DDA) dragging
    property is true, the DDA stays in that state and doesn't recover any more.
*/
void tst_UCSwipeArea::disabledWhileDragging()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF touchPoint = calculateInitialtouchPosition(edgeDragArea);

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;
    QPointF dragDirectionVector(1., 0.); // horizontal positive

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);
    QCOMPARE(edgeDragArea->dragging(), true);

    // disable the swipeArea while it's being dragged.
    edgeDragArea->setEnabled(false);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);
    QCOMPARE(edgeDragArea->dragging(), false);

    QTest::touchEvent(m_view, m_device).release(0, touchPoint.toPoint(), (QWindow*)nullptr);
}

void tst_UCSwipeArea::oneFingerDownFollowedByLateSecondFingerDown()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // Disable some constraints we're not interested in
    d->setMaxTime(60 * 1000);

    // And ensure others have the values we want
    d->compositionTime = 60;

    // Put an item right behind edgeDragArea to receive the touches ignored by it
    DummyItem *dummyItem = new DummyItem;
    dummyItem->setParentItem(edgeDragArea->parentItem());
    dummyItem->setX(edgeDragArea->x());
    dummyItem->setY(edgeDragArea->y());
    dummyItem->setZ(edgeDragArea->z() - 1.0);
    dummyItem->setWidth(edgeDragArea->width());
    dummyItem->setHeight(edgeDragArea->height());

    // Make touches evenly spaced along the edgeDragArea
    QPoint touch0Pos(edgeDragArea->width()/2.0f, m_view->height()*0.33f);
    QPoint touch1Pos(edgeDragArea->width()/2.0f, m_view->height()*0.66f);

    QTest::touchEvent(m_view, m_device).press(0, touch0Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    // We are now going to be way beyond compositionTime
    passTime(d->compositionTime*3);

    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .press(1, touch1Pos, (QWindow*)nullptr);

    // A new touch has come, but as it can't be composed with touch 0, it should be
    // ignored/rejected by the UCSwipeArea
    // Therefore the last event received by dummyItem must have both touch points (0 and 1)
    {
        TouchMemento &touchMemento = dummyItem->touchEvents.last();
        QCOMPARE(touchMemento.touchPoints.count(), 2);
        QVERIFY(touchMemento.containsTouchWithId(0));
        QVERIFY(touchMemento.containsTouchWithId(1));
    }

    passTime(30);

    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .move(1, touch1Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    passTime(5);

    QTest::touchEvent(m_view, m_device)
        .release(0, touch0Pos, (QWindow*)nullptr)
        .move(1, touch1Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(5);

    QTest::touchEvent(m_view, m_device)
        .release(1, touch1Pos, (QWindow*)nullptr);

    // Shouldn't be keepping info about touches that no longer exist or interest us
    QVERIFY(d->activeTouches.isEmpty());

    delete dummyItem;
}

void tst_UCSwipeArea::givesUpWhenLosesTouch()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // Disable some constraints we're not interested in
    d->setMaxTime(60 * 1000);

    // Put an item right in front of edgeDragArea
    DummyItem *dummyItem = new DummyItem(edgeDragArea->parentItem());
    dummyItem->setX(edgeDragArea->x());
    dummyItem->setY(edgeDragArea->y());
    dummyItem->setZ(edgeDragArea->z() + 1.0);
    dummyItem->setWidth(edgeDragArea->width());
    dummyItem->setHeight(edgeDragArea->height());

    QPoint touchPosition(edgeDragArea->width()/2.0f, m_view->height()/2.0f);

    dummyItem->touchEventHandler = [&](QTouchEvent *event) {
        m_touchRegistry->addCandidateOwnerForTouch(0, dummyItem);
        event->ignore();
    };

    QTest::touchEvent(m_view, m_device).press(0, touchPosition, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    m_touchRegistry->requestTouchOwnership(0, dummyItem);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    dummyItem->grabTouchPoints({0});
    dummyItem->touchEventHandler = [&](QTouchEvent *event) { event->accept(); };

    passTime(5);
    QTest::touchEvent(m_view, m_device).release(0, touchPosition, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    QVERIFY(d->activeTouches.isEmpty());
}

void tst_UCSwipeArea::threeFingerDrag()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // Disable some constraints we're not interested in
    d->setMaxTime(60 * 1000);

    // And ensure others have the values we want
    d->compositionTime = 60;

    // Make touches evenly spaced along the edgeDragArea
    QPoint touch0Pos(edgeDragArea->width()/2.0f, m_view->height()*0.25f);
    QPoint touch1Pos(edgeDragArea->width()/2.0f, m_view->height()*0.50f);
    QPoint touch2Pos(edgeDragArea->width()/2.0f, m_view->height()*0.75f);

    QTest::touchEvent(m_view, m_device)
        .press(0, touch0Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    passTime(5);
    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .press(1, touch1Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(5);
    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .move(1, touch1Pos, (QWindow*)nullptr)
        .press(2, touch2Pos, (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    passTime(10);
    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .move(1, touch1Pos, (QWindow*)nullptr)
        .move(2, touch2Pos, (QWindow*)nullptr);

    passTime(200);
    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .release(1, touch1Pos, (QWindow*)nullptr)
        .move(2, touch2Pos, (QWindow*)nullptr);

    passTime(10);
    QTest::touchEvent(m_view, m_device)
        .move(0, touch0Pos, (QWindow*)nullptr)
        .release(2, touch2Pos, (QWindow*)nullptr);

    passTime(5);
    QTest::touchEvent(m_view, m_device)
        .release(0, touch0Pos, (QWindow*)nullptr);

    // Shouldn't be keepping info about touches that no longer exist or interest us
    QVERIFY(d->activeTouches.isEmpty());
}

/*
   If all the relevant gesture recognition constraints/parameters have been disabled,
   it means that the gesture recognition itself has been disabled and UCSwipeArea
   will therefore work like a simple touch area, merely reporting touch movement.
 */
void tst_UCSwipeArea::immediateRecognitionWhenConstraintsDisabled()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    // Disable the minimum amount of constraints to ensure immediate recognition
    d->setDistanceThreshold(0);
    d->compositionTime = 0;

    // Put an item right behind edgeDragArea to receive the touches ignored by it
    DummyItem *dummyItem = new DummyItem;
    dummyItem->setParentItem(edgeDragArea->parentItem());
    dummyItem->setX(edgeDragArea->x());
    dummyItem->setY(edgeDragArea->y());
    dummyItem->setZ(edgeDragArea->z() - 1.0);
    dummyItem->setWidth(edgeDragArea->width());
    dummyItem->setHeight(edgeDragArea->height());

    QPoint touch0Pos(edgeDragArea->width()/2.0f, m_view->height()/2.0f);

    QSignalSpy pressedSpy(edgeDragArea, &UCSwipeArea::pressedChanged);
    QCOMPARE(edgeDragArea->pressed(), false);

    QTest::touchEvent(m_view, m_device).press(0, touch0Pos, (QWindow*)nullptr);

    // check for immediate recognition
    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    // and it is pressed
    QCOMPARE(edgeDragArea->pressed(), true);
    QCOMPARE(pressedSpy.count(), 1);

    // and therefore it should have immediately grabbed the touch point,
    // not letting it leak to items behind him.
    QCOMPARE(dummyItem->touchEvents.count(), 0);
}

void tst_UCSwipeArea::withdrawTouchOwnershipCandidacyIfDisabledDuringRecognition()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF touchPoint = calculateInitialtouchPosition(edgeDragArea);

    // Move less than the minimum needed for the drag gesture recognition
    qreal desiredDragDistance = d->distanceThreshold * 0.5f;
    QPointF dragDirectionVector(1., 0.); // horizontal positive

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 0.8f) / totalMovementSteps;

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);

    // edgeDragArea should be an undecided candidate
    {
        auto touchInfo = m_touchRegistry->findTouchInfo(0);
        QCOMPARE(touchInfo->candidates.size(), 1);
        QCOMPARE(touchInfo->candidates.at(0).item.data(), edgeDragArea);
        QCOMPARE(touchInfo->candidates.at(0).state, TouchRegistry::CandidateInfo::Undecided);
    }

    // disable the swipeArea while it's still recognizing a possible drag gesture.
    QFETCH(bool, disable);
    if (disable) {
        edgeDragArea->setEnabled(false);
    } else {
        edgeDragArea->setVisible(false);
    }

    // edgeDragArea should no longer be a candidate
    {
        auto touchInfo = m_touchRegistry->findTouchInfo(0);
        QCOMPARE(touchInfo->candidates.size(), 0);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    QTest::touchEvent(m_view, m_device).release(0, touchPoint.toPoint(), (QWindow*)nullptr);
}

void tst_UCSwipeArea::withdrawTouchOwnershipCandidacyIfDisabledDuringRecognition_data()
{
    QTest::addColumn<bool>("disable");

    QTest::newRow("disabled") << true;
    QTest::newRow("invisible") << false;
}

void tst_UCSwipeArea::gettingTouchOwnershipMakesMouseAreaBehindGetCanceled()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != nullptr);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QQuickMouseArea *mouseArea =
        m_view->rootObject()->findChild<QQuickMouseArea*>("mouseArea");
    QVERIFY(mouseArea != nullptr);

    MouseAreaSpy mouseAreaSpy(mouseArea);

    QPointF initialtouchPosition = calculateInitialtouchPosition(edgeDragArea);
    QPointF touchPoint = initialtouchPosition;

    qreal desiredDragDistance = d->distanceThreshold * 2;
    QPointF dragDirectionVector(1.0f, 0.0f); // rightwards
    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    QCOMPARE(mouseArea->pressed(), false);

    QTest::touchEvent(m_view, m_device).press(0, touchPoint.toPoint(), (QWindow*)nullptr);

    // The TouchBegin passes through the UCSwipeArea and reaches the MouseArea behind it,
    // where it's converted to a MouseEvent by QQuickWindow and sent to the MouseArea which then
    // accepts it. Making it get pressed.
    QCOMPARE(mouseArea->pressed(), true);
    QCOMPARE(mouseAreaSpy.canceledCount, 0);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touchPoint.toPoint(), (QWindow*)nullptr);
    }

    // As the UCSwipeArea recognizes the gesture, it grabs the touch from the MouseArea,
    // which should make the MouseArea get a cancelation event, which will then cause it to
    // reset its state (going back to "unpressed"/"released").
    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);
    QCOMPARE(mouseArea->pressed(), false);
    QCOMPARE(mouseAreaSpy.canceledCount, 1);

    QTest::touchEvent(m_view, m_device).release(0, touchPoint.toPoint(), (QWindow*)nullptr);
}

void tst_UCSwipeArea::interleavedTouches()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF touch0 = edgeDragArea->mapToScene(
            QPointF(edgeDragArea->width()*0.5, edgeDragArea->height()*0.3));

    qreal desiredDragDistance = d->distanceThreshold * 2;
    QPointF dragDirectionVector(1.0f, 0.0f); // rightwards
    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->maxTime * 0.4f) / totalMovementSteps;

    QTest::touchEvent(m_view, m_device).press(0, touch0.toPoint(), (QWindow*)nullptr);
    for (int i = 0; i < totalMovementSteps; ++i) {
        touch0 += touchMovement;
        passTime(movementTimeStepMs);
        QTest::touchEvent(m_view, m_device).move(0, touch0.toPoint(), (QWindow*)nullptr);
    }
    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    QPointF touch1 = edgeDragArea->mapToScene(
            QPointF(edgeDragArea->width()*0.5, edgeDragArea->height()*0.6));

    QTest::touchEvent(m_view, m_device)
        .move(0, touch0.toPoint(), (QWindow*)nullptr)
        .press(1, touch1.toPoint(), (QWindow*)nullptr);

    touch1 += touchMovement;
    passTime(movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .move(0, touch0.toPoint(), (QWindow*)nullptr)
        .move(1, touch1.toPoint(), (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);

    QTest::touchEvent(m_view, m_device)
        .release(0, touch0.toPoint(), (QWindow*)nullptr)
        .move(1, touch1.toPoint(), (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    touch1 += touchMovement;
    passTime(movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .move(1, touch1.toPoint(), (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);

    QPointF touch2 = edgeDragArea->mapToScene(
            QPointF(edgeDragArea->width()*0.5, edgeDragArea->height()*0.9));

    passTime(d->compositionTime + movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .move(1, touch1.toPoint(), (QWindow*)nullptr)
        .press(2, touch2.toPoint(), (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Undecided);
    QCOMPARE(d->touchId, 2);

    touch2 += touchMovement;
    passTime(movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .move(1, touch1.toPoint(), (QWindow*)nullptr)
        .move(2, touch2.toPoint(), (QWindow*)nullptr);

    touch1 += touchMovement;
    passTime(movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .move(1, touch1.toPoint(), (QWindow*)nullptr)
        .move(2, touch2.toPoint(), (QWindow*)nullptr);

    passTime(movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .release(1, touch1.toPoint(), (QWindow*)nullptr)
        .move(2, touch2.toPoint(), (QWindow*)nullptr);

    passTime(movementTimeStepMs);
    QTest::touchEvent(m_view, m_device)
        .release(2, touch2.toPoint(), (QWindow*)nullptr);

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::WaitingForTouch);
}

/*
  A valid right-edge drag performed on mako
 */
void tst_UCSwipeArea::makoRightEdgeDrag()
{
    m_view->resize(768, 1280);
    QTest::qWait(300);

    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hnDragArea");
    QVERIFY(edgeDragArea != nullptr);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    StatusSpy *statusSpy = new StatusSpy(edgeDragArea);

    d->setPixelsPerMm(320.0 /*mako ppi*/ * 0.03937 /* inches per mm*/);

    sendTouchPress(319744, 0, QPointF(767.001, 719.719));
    sendTouchUpdate(319765, 0, QPointF(765.744,729.973));
    sendTouchUpdate(319784, 0, QPointF(740.879,752.182));
    sendTouchUpdate(319803, 0, QPointF(689.698,795.795));
    sendTouchUpdate(319826, 0, QPointF(616.978,856.212));
    sendTouchUpdate(319845, 0, QPointF(558.769,906.157));
    sendTouchUpdate(319859, 0, QPointF(513.219,945.266));
    sendTouchUpdate(319878, 0, QPointF(481.31,975.496));
    sendTouchUpdate(319902, 0, QPointF(460.016,997.439));
    sendTouchUpdate(319920, 0, QPointF(449.761,1008.6));
    sendTouchUpdate(319929, 0, QPointF(445.891,1012.42));
    sendTouchUpdate(319947, 0, QPointF(444.884,1013.93));
    sendTouchUpdate(319965, 0, QPointF(444.461,1014.35));
    sendTouchUpdate(320057, 0, QPointF(444.71,1013.56));
    sendTouchUpdate(320138, 0, QPointF(445.434,1013.6));
    sendTouchUpdate(320154, 0, QPointF(446.338,1012.98));
    sendTouchUpdate(320171, 0, QPointF(447.232,1012.08));
    sendTouchRelease(320171, 0, QPointF(447.232,1012.08));

    QTRY_COMPARE_WITH_TIMEOUT(statusSpy->recognized(), true, 500);

    delete statusSpy;
}

/*
   A vertical, downwards swipe performed on mako near its right edge.

   The UCSwipeArea on the right edge must not recognize this
   gesture.
 */
void tst_UCSwipeArea::makoRightEdgeDrag_verticalDownwards()
{
    m_view->resize(768, 1280);
    QTest::qWait(300);

    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hnDragArea");
    QVERIFY(edgeDragArea != nullptr);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    d->setPixelsPerMm(320.0 /*mako ppi*/ * 0.03937 /* inches per mm*/);

    StatusSpy *statusSpy = new StatusSpy(edgeDragArea);

    sendTouchPress(12012445, 26, QPointF(767.001,461.82));
    sendTouchUpdate(12012472, 26, QPointF(767.001,462.569));
    sendTouchUpdate(12012528, 26, QPointF(767.001,463.334));
    sendTouchUpdate(12012546, 26, QPointF(767.001,466.856));
    sendTouchUpdate(12012571, 26, QPointF(767.001,473.291));
    sendTouchUpdate(12012587, 26, QPointF(767.001,487.31));
    sendTouchUpdate(12012604, 26, QPointF(765.364,507.521));
    sendTouchUpdate(12012618, 26, QPointF(765.364,507.521));
    sendTouchUpdate(12012627, 26, QPointF(762.642,534.317));
    sendTouchUpdate(12012655, 26, QPointF(760.846,573.406));
    sendTouchUpdate(12012667, 26, QPointF(759.838,625.295));
    sendTouchUpdate(12012675, 26, QPointF(758.875,703.207));
    sendTouchUpdate(12012696, 26, QPointF(761.52,777.015));
    sendTouchUpdate(12012713, 26, QPointF(765.659,835.591));
    sendTouchUpdate(12012731, 26, QPointF(766.778,883.206));
    sendTouchUpdate(12012748, 26, QPointF(767.001,922.937));
    sendTouchUpdate(12012779, 26, QPointF(767.001,967.558));
    sendTouchUpdate(12012798, 26, QPointF(767.001,1006.12));
    sendTouchUpdate(12012809, 26, QPointF(767.001,1033.1));
    sendTouchRelease(12012810, 26, QPointF(767.001,1033.1));

    QCOMPARE(statusSpy->recognized(), false);

    delete statusSpy;
}

/*
   A valid left-edge drag performed on mako. This one starts a bit slow than speeds up
 */
void tst_UCSwipeArea::makoLeftEdgeDrag_slowStart()
{
    m_view->resize(768, 1280);
    QTest::qWait(300);

    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != nullptr);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    d->setPixelsPerMm(320.0 /*mako ppi*/ * 0.03937 /* inches per mm*/);

    StatusSpy *statusSpy = new StatusSpy(edgeDragArea);

    sendTouchPress(4002267, 77, QPointF(0,885.154));
    sendTouchUpdate(4002275, 77, QPointF(0,886.214));
    sendTouchUpdate(4002311, 77, QPointF(1.09568,887.75));
    sendTouchUpdate(4002329, 77, QPointF(3.53647,890.191));
    sendTouchUpdate(4002347, 77, QPointF(7.87434,892.879));
    sendTouchUpdate(4002366, 77, QPointF(12.3036,895.075));
    sendTouchUpdate(4002384, 77, QPointF(15.8885,896.849));
    sendTouchUpdate(4002406, 77, QPointF(18.4504,897.88));
    sendTouchUpdate(4002420, 77, QPointF(20.2429,898.149));
    sendTouchUpdate(4002439, 77, QPointF(20.9945,898.149));
    sendTouchUpdate(4002457, 77, QPointF(21.8819,898.149));
    sendTouchUpdate(4002480, 77, QPointF(22.7454,897.389));
    sendTouchUpdate(4002493, 77, QPointF(23.5456,896.589));
    sendTouchUpdate(4002511, 77, QPointF(24.5435,895.031));
    sendTouchUpdate(4002529, 77, QPointF(25.4271,892.32));
    sendTouchUpdate(4002548, 77, QPointF(26.3145,889.658));
    sendTouchUpdate(4002566, 77, QPointF(27.2004,886.999));
    sendTouchUpdate(4002584, 77, QPointF(28.035,885.048));
    sendTouchUpdate(4002603, 77, QPointF(29.9684,883.167));
    sendTouchUpdate(4002620, 77, QPointF(33.3591,881.403));
    sendTouchUpdate(4002639, 77, QPointF(44.1017,879.642));
    sendTouchUpdate(4002657, 77, QPointF(64.828,878.502));
    sendTouchUpdate(4002675, 77, QPointF(87.9486,878.157));
    sendTouchUpdate(4002693, 77, QPointF(112.96,877.742));
    sendTouchUpdate(4002711, 77, QPointF(138.903,877.157));
    sendTouchUpdate(4002729, 77, QPointF(163.204,877.157));
    sendTouchUpdate(4002747, 77, QPointF(182.127,877.157));
    sendTouchUpdate(4002765, 77, QPointF(194.478,877.657));
    sendTouchUpdate(4002785, 77, QPointF(201.474,878.508));
    sendTouchUpdate(4002803, 77, QPointF(204.855,879.401));
    sendTouchUpdate(4002822, 77, QPointF(206.616,880.281));
    sendTouchUpdate(4002839, 77, QPointF(207.115,880.906));
    sendTouchUpdate(4002894, 77, QPointF(206.865,881.184));
    sendTouchUpdate(4002912, 77, QPointF(206.865,882.143));
    sendTouchUpdate(4002930, 77, QPointF(206.865,883.106));
    sendTouchUpdate(4002949, 77, QPointF(206.526,883.994));
    sendTouchUpdate(4002967, 77, QPointF(205.866,884.88));
    sendTouchUpdate(4002985, 77, QPointF(205.866,885.766));
    sendTouchUpdate(4003005, 77, QPointF(205.866,886.654));
    sendTouchUpdate(4003021, 77, QPointF(205.366,887.537));
    sendTouchUpdate(4003039, 77, QPointF(204.592,888.428));
    sendTouchUpdate(4003050, 77, QPointF(204.367,888.653));
    sendTouchRelease(4003050, 77, QPointF(204.367,888.653));

    QCOMPARE(statusSpy->recognized(), true);

    delete statusSpy;
}

void tst_UCSwipeArea::makoLeftEdgeDrag_movesSlightlyBackwardsOnStart()
{
    m_view->resize(768, 1280);
    QTest::qWait(300);

    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("hpDragArea");
    QVERIFY(edgeDragArea != nullptr);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    d->setPixelsPerMm(320.0 /*mako ppi*/ * 0.03937 /* inches per mm*/);

    StatusSpy *statusSpy = new StatusSpy(edgeDragArea);

    sendTouchPress(41097, 24, QPointF(13.9909,827.177));
    sendTouchUpdate(41120, 24, QPointF(19.2375,825.677));
    sendTouchUpdate(41138, 24, QPointF(18.4057,826.177));
    sendTouchUpdate(41161, 24, QPointF(20.1067,825.867));
    sendTouchUpdate(41177, 24, QPointF(21.8869,824.977));
    sendTouchUpdate(41193, 24, QPointF(24.7603,823.494));
    sendTouchUpdate(41211, 24, QPointF(28.3889,821.725));
    sendTouchUpdate(41229, 24, QPointF(32.2909,819.955));
    sendTouchUpdate(41247, 24, QPointF(38.2251,817.431));
    sendTouchUpdate(41266, 24, QPointF(52.4182,814.223));
    sendTouchUpdate(41284, 24, QPointF(85.8465,809.483));
    sendTouchUpdate(41302, 24, QPointF(126.091,802.741));
    sendTouchUpdate(41320, 24, QPointF(153.171,797.977));
    sendTouchUpdate(41338, 24, QPointF(170.565,795.077));
    sendTouchUpdate(41356, 24, QPointF(178.685,794.101));
    sendTouchUpdate(41375, 24, QPointF(183.706,793.225));
    sendTouchUpdate(41393, 24, QPointF(186.112,793.19));
    sendTouchUpdate(41411, 24, QPointF(187.634,793.19));
    sendTouchUpdate(41429, 24, QPointF(188.505,793.19));
    sendTouchUpdate(41532, 24, QPointF(187.816,793.19));
    sendTouchUpdate(41538, 24, QPointF(186.902,793.19));
    sendTouchUpdate(41557, 24, QPointF(186.01,793.19));
    sendTouchUpdate(41575, 24, QPointF(185.125,793.444));
    sendTouchUpdate(41593, 24, QPointF(184.229,793.69));
    sendTouchUpdate(41605, 24, QPointF(183.88,793.69));
    sendTouchRelease(41607, 24, QPointF(183.88,793.69));

    QCOMPARE(statusSpy->recognized(), true);

    delete statusSpy;
}

void tst_UCSwipeArea::grabGesture()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("vpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);
    d->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragArea));
    d->setTimeSource(m_fakeTimerFactory->timeSource());

    UCSwipeArea *edgeDragAreaNoGrab =
        m_view->rootObject()->findChild<UCSwipeArea*>("vpDragAreaNoGrab");
    Q_ASSERT(edgeDragAreaNoGrab != 0);
    QCOMPARE(edgeDragAreaNoGrab->grabGesture(), false);
    UCSwipeAreaPrivate *dNoGrab = UCSwipeAreaPrivate::get(edgeDragAreaNoGrab);
    dNoGrab->setRecognitionTimer(m_fakeTimerFactory->createTimer(edgeDragAreaNoGrab));
    dNoGrab->setTimeSource(m_fakeTimerFactory->timeSource());

    QPointF touchPoint = calculateInitialtouchPosition(edgeDragArea);

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;
    QPointF dragDirectionVector(0., 1.); // vertical positive

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    qint64 timestamp = 0;

    sendTouchPress(timestamp, 0, touchPoint);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        timestamp += movementTimeStepMs;
        sendTouchUpdate(timestamp, 0, touchPoint);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);
    QCOMPARE(edgeDragArea->dragging(), true);

    QCOMPARE((int)dNoGrab->status, (int)UCSwipeAreaPrivate::Recognized);
    QCOMPARE(edgeDragAreaNoGrab->dragging(), true);

    // Use a large distance margin because position smoothing makes it hard
    // to assume high accuracy.  The test below using immediateRecognition uses
    // a smaller margin.
    // NB: qFuzzyCompare(), used internally by QCOMPARE(), is broken.
    QVERIFY(qAbs(edgeDragArea->distance() - edgeDragAreaNoGrab->distance()) < 2);

    timestamp += movementTimeStepMs;
    sendTouchRelease(timestamp, 0, touchPoint);
}

void tst_UCSwipeArea::grabGestureWithImmediateRecognition()
{
    UCSwipeArea *edgeDragArea =
        m_view->rootObject()->findChild<UCSwipeArea*>("vpDragArea");
    Q_ASSERT(edgeDragArea != 0);
    UCSwipeAreaPrivate *d = UCSwipeAreaPrivate::get(edgeDragArea);

    UCSwipeArea *edgeDragAreaNoGrab =
        m_view->rootObject()->findChild<UCSwipeArea*>("vpDragAreaNoGrab");
    Q_ASSERT(edgeDragAreaNoGrab != 0);
    QCOMPARE(edgeDragAreaNoGrab->grabGesture(), false);
    UCSwipeAreaPrivate *dNoGrab = UCSwipeAreaPrivate::get(edgeDragAreaNoGrab);

    edgeDragArea->setImmediateRecognition(true);
    edgeDragAreaNoGrab->setImmediateRecognition(true);

    QPointF touchPoint = calculateInitialtouchPosition(edgeDragArea);

    qreal desiredDragDistance = d->distanceThreshold * 2.0f;
    QPointF dragDirectionVector(0., 1.); // vertical positive

    qreal movementStepDistance = d->distanceThreshold * 0.1f;
    QPointF touchMovement = dragDirectionVector * movementStepDistance;
    int totalMovementSteps = qCeil(desiredDragDistance / movementStepDistance);
    int movementTimeStepMs = (d->compositionTime * 1.5f) / totalMovementSteps;

    qint64 timestamp = 0;

    sendTouchPress(timestamp, 0, touchPoint);

    for (int i = 0; i < totalMovementSteps; ++i) {
        touchPoint += touchMovement;
        timestamp += movementTimeStepMs;
        sendTouchUpdate(timestamp, 0, touchPoint);
    }

    QCOMPARE((int)d->status, (int)UCSwipeAreaPrivate::Recognized);
    QCOMPARE(edgeDragArea->dragging(), true);

    QCOMPARE((int)dNoGrab->status, (int)UCSwipeAreaPrivate::Recognized);
    QCOMPARE(edgeDragAreaNoGrab->dragging(), true);

    // NB: qFuzzyCompare(), used internally by QCOMPARE(), is broken.
    QVERIFY(qAbs(edgeDragArea->distance() - edgeDragAreaNoGrab->distance()) < 0.001);

    timestamp += movementTimeStepMs;
    sendTouchRelease(timestamp, 0, touchPoint);
}

QTEST_MAIN(tst_UCSwipeArea)

#include "tst_swipearea.moc"
