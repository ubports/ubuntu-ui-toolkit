/*
 * Copyright 2013 Canonical Ltd.
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

#include "ucubuntuanimation.h"
#include <QtCore/QPointF>

UT_NAMESPACE_BEGIN

/*!
 * \qmltype UbuntuAnimation
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief Singleton defining standard Ubuntu durations and easing for animations
 *        that should be used to ensure that Ubuntu applications are consistent
 *        in their animations.
 *
 * Example of use:
 *
 * \qml
 * RotationAnimation {
 *    duration: UbuntuAnimation.SlowDuration
 *    easing: UbuntuAnimation.StandardEasing
 * }
 * \endqml
 *
 * Animation durations should be selected depending on the frequency and
 * disruptiveness of the animation. The more frequent an animation is, the
 * faster it should be. The more disruptive an animation is, the slower it should
 * be. Rule of thumb to select durations:
 *  \list
 *   \li SnapDuration: very frequent, non-disruptive.
 *   \li FastDuration: frequent, non-disruptive.
 *   \li SlowDuration: less frequent, non-disruptive.
 *   \li SleepyDuration: disruptive.
 *  \endlist
 *
 * Note that \l UbuntuNumberAnimation provides a standard NumberAnimation for
 * Ubuntu applications.
 *
 */

UCUbuntuAnimation::UCUbuntuAnimation(QObject *parent) :
    QObject(parent),
    m_standardEasing(QEasingCurve::OutQuad),
    m_StandardEasingReverse(QEasingCurve::InQuad)
{
}

/*!
 * \qmlproperty int UbuntuAnimation::SnapDuration
 * Used for very frequent and non-disruptive transitions on small objects.
 * The user would perceive the change as instant but smooth.
 *
 * The value is 100ms.
 */

/*!
 * \qmlproperty int UbuntuAnimation::FastDuration
 * Used for frequent and non-disruptive transitions.
 *
 * The value is 165ms.
 */

/*!
 * \qmlproperty int UbuntuAnimation::BriskDuration
 * Used for frequent and non-disruptive transitions.
 * Used when objects have more distance to travel or when they are larger in size.
 *
 * The value is 333ms.
 */

/*!
 * \qmlproperty int UbuntuAnimation::SlowDuration
 * Used for delay after key press and for less frequent and non-disruptive
 * transitions.
 *
 * The value is 500ms.
 */

/*!
 * \qmlproperty int UbuntuAnimation::SleepyDuration
 * Used for disruptive transitions.
 *
 * The value is 1000ms.
 */

/*!
 * \qmlproperty QEasingCurve UbuntuAnimation::StandardEasing
 * Used for animations trigerred by user actions.
 */

/*!
 * \qmlproperty QEasingCurve UbuntuAnimation::StandardEasingReverse
 *
 * StandardEasingReverse is mainly used in combination with StandardEasing.
 * For example, if animating an object that bounces, you will want the object
 * to slow down as it reaches the apex of its bounce and slowly speed back up
 * as it descends.
 *
 * StandardEasingReverse should not be used to introduce a new object or screen
 * to the user.
 * In general, it should only be used if StandardEasing is visually inappropriate
 * and even so, will usually be proceeded by StandardEasing.
 */

UT_NAMESPACE_END
