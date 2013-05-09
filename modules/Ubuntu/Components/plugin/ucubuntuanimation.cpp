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

/*!
 * \qmltype UbuntuAnimation
 * \instantiates UCUbuntuAnimation
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief This is a singleton type defining standard Ubuntu durations and
 *        easing for animations.
 *
 * Example of use:
 *
 * \qml
 * NumberAnimation {
 *    duration: UbuntuAnimation.SlowDuration
 *    easing: UbuntuAnimation.StandardEasing
 * }
 * \endqml
 *
 */

UCUbuntuAnimation::UCUbuntuAnimation(QObject *parent) :
    QObject(parent)
{
}

/*!
 * \qmlproperty real UbuntuAnimation::SnapDuration
 * Used for very frequent and non-disruptive transitions on small objects.
 * The user would perceive the change as instant but smooth.
 *
 * The value is 125ms.
 */

/*!
 * \qmlproperty real UbuntuAnimation::FastDuration
 * Used for frequent and non-disruptive transitions.
 *
 * The value is 250ms.
 */

/*!
 * \qmlproperty real UbuntuAnimation::SlowDuration
 * Used for delay after key press and for custom (less frequent) and non-disruptive
 * transitions (also used for undo animations like on mouse exit).
 *
 * The value is 500ms.
 */

/*!
 * \qmlproperty real UbuntuAnimation::SleepyDuration
 * Used for disruptive transitions.
 *
 * The value is 1000ms.
 */

/*!
 * \qmlproperty QEasingCurve UbuntuAnimation::StandardEasing
 * Used for animations trigerred by user actions.
 *
 * The type is Easing.OutQuint.
 */
