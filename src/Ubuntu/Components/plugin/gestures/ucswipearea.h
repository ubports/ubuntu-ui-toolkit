/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef UCSWIPEAREA_H
#define UCSWIPEAREA_H

#include <QtQuick/QQuickItem>
#include "ubuntugesturesqmlglobal.h"
#include "damper.h"

// lib UbuntuGestures
#include <UbuntuGestures/Pool>
#include <UbuntuGestures/Timer>

class TouchOwnershipEvent;
class UnownedTouchEvent;
class UCSwipeAreaPrivate;

/*
 An area that detects axis-aligned single-finger drag gestures

 If a drag deviates too much from the components' direction recognition will
 fail. It will also fail if the drag or flick is too short. E.g. a noisy or
 fidgety click

 See doc/DirectionalDragArea.svg
 */
class UBUNTUGESTURESQML_EXPORT UCSwipeArea : public QQuickItem
{
    Q_OBJECT

    // The direction in which the gesture should move in order to be recognized.
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)

    // The distance travelled by the finger along the axis specified by
    // UCSwipeArea's direction.
    Q_PROPERTY(qreal distance READ distance NOTIFY distanceChanged)

    // The distance travelled by the finger along the axis specified by
    // UCSwipeArea's direction in scene coordinates
    Q_PROPERTY(qreal sceneDistance READ sceneDistance NOTIFY sceneDistanceChanged)

    // Position of the touch point performing the drag relative to this item.
    Q_PROPERTY(QPointF touchPosition READ touchPosition NOTIFY touchPositionChanged)

    // Position of the touch point performing the drag, in scene's coordinate system
    Q_PROPERTY(QPointF touchScenePosition READ touchScenePosition NOTIFY touchScenePositionChanged)

    // Whether a drag gesture is taking place
    Q_PROPERTY(bool dragging READ dragging NOTIFY draggingChanged)

    // Whether the drag area is pressed.
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)

    // Whether a gesture should be Recognized as soon a touch lands on the area.
    // With this property enabled it will work pretty much like a MultiPointTouchArea,
    // just with a different API.
    //
    // It's false by default. In most cases you will not want that enabled.
    Q_PROPERTY(bool immediateRecognition
            READ immediateRecognition
            WRITE setImmediateRecognition
            NOTIFY immediateRecognitionChanged)

    Q_ENUMS(Direction)
public:
    enum Direction {
        Rightwards, // Along the positive direction of the X axis
        Leftwards, // Along the negative direction of the X axis
        Downwards, // Along the positive direction of the Y axis
        Upwards, // Along the negative direction of the Y axis
        Horizontal, // Along the X axis, in any direction
        Vertical // Along the Y axis, in any direction
    };

    UCSwipeArea(QQuickItem *parent = 0);

    Direction direction() const;
    void setDirection(Direction);

    qreal distance() const;
    qreal sceneDistance() const;

    QPointF touchPosition() const;

    QPointF touchScenePosition() const;

    bool dragging() const;

    bool pressed() const;

    bool immediateRecognition() const;
    void setImmediateRecognition(bool enabled);

    bool event(QEvent *e) override;

    /*
      In qmltests, sequences of touch events are sent all at once, unlike in "real life".
      Also qmltests might run really slowly, e.g. when run from inside virtual machines.
      Thus to remove a variable that qmltests cannot really control, namely time, this
      function removes all constraints that are sensible to elapsed time.

      This effectively makes the UCSwipeArea easier to fool.
     */
    Q_INVOKABLE void removeTimeConstraints();

Q_SIGNALS:
    void directionChanged(Direction direction);
    void draggingChanged(bool value);
    void pressedChanged(bool value);
    void distanceChanged(qreal value);
    void sceneDistanceChanged(qreal value);
    void touchPositionChanged(const QPointF &pos);
    void touchScenePositionChanged(const QPointF &pos);
    void immediateRecognitionChanged(bool value);

protected:
    void touchEvent(QTouchEvent *event) override;
    void itemChange(ItemChange change, const ItemChangeData &value) override;

public: // so tests can access it
    UCSwipeAreaPrivate *d;
};

#endif // UCSWIPEAREA_H
