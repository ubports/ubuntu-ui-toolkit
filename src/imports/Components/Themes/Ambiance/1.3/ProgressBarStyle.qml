/*
 * Copyright 2016 Canonical Ltd.
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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Rectangle {
    id: progressBarStyle

    property color foregroundColor: styledItem.enabled
                                        ? theme.palette.normal.activity
                                        : theme.palette.disabled.activity
    color: styledItem.enabled
                ? theme.palette.normal.base
                : theme.palette.disabled.base

    implicitWidth: units.gu(36)
    implicitHeight: units.gu(0.4)
    clip: true

    property real progress: styledItem.indeterminate ? 0.0
                            : styledItem.value / (styledItem.maximumValue - styledItem.minimumValue)

    Rectangle {
        id: strip
        anchors {
            top: parent.top
            left: !styledItem.indeterminate ? parent.left : undefined
            bottom: parent.bottom
        }
        color: foregroundColor
        width: styledItem.indeterminate
                    ? MathUtils.clamp(parent.width / 4, units.gu(1), units.gu(30))
                    : parent.width * progressBarStyle.progress

        property int duration: UbuntuAnimation.SleepyDuration
        property int easing: Easing.InOutQuad
        readonly property bool animateStrip:
            styledItem.enabled && styledItem.visible
            && styledItem.indeterminate
            // animate only after style completion!
            && animated && !reverseAnimate
        property bool reverseAnimate: false

        NumberAnimation on width {
            duration: UbuntuAnimation.SlowDuration
            easing.type: strip.easing
            running: !styledItem.indeterminate && styledItem.enabled && styledItem.visible
        }

        state: animateStrip ? "animate" : ""

        // reversible doesn't work, so we need animatiomns for both transitions
        // note: we cannot use XAnimator as there will be a point when
        // there will be two animators altering (running) the x property at the
        // same time, which causes SIGSEG
        transitions: [
            Transition {
                from: ""
                to: "animate"
                SequentialAnimation {
                    NumberAnimation {
                        target: strip
                        property: "x"
                        duration: strip.duration
                        easing.type: strip.easing
                        from: -strip.width
                        to: progressBarStyle.width
                    }
                    ScriptAction { script: strip.reverseAnimate = true; }
                }
            },
            Transition {
                from: "animate"
                to: ""
                SequentialAnimation {
                    NumberAnimation {
                        target: strip
                        property: "x"
                        duration: strip.duration
                        easing.type: strip.easing
                        to: -strip.width
                        from: progressBarStyle.width
                    }
                    ScriptAction { script: strip.reverseAnimate = false; }
                }
            }
        ]
    }
}
