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
 */

import QtQuick 2.4
import QtQuick.Window 2.0
import Ubuntu.Components 1.2

/*!
    \qmltype OrientationHelper
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief The OrientationHelper automatically rotates its children following the
           orientation of the device.

    Any Item placed inside an Orientation Helper will be automatically rotated
    following the orientation of the device.

    Note that OrientationHelper is always filling its parent (anchors.parent: fill).

    Example:
    \qml
    Item {
        OrientationHelper {
            Label {
                text: "Automatically rotated"
            }
            Button {
                text: "Automatically rotated"
            }
        }
    }
    \endqml
*/
Item {
    id: orientationHelper

    /*!
      Sets whether it will be automatically rotating when the device is.

      The default value is true.

      \qmlproperty bool automaticOrientation
     */
    property bool automaticOrientation: true
    /*!
      Sets whether the rotation transition is performed.

      The default value is true.

      \qmlproperty bool transitionEnabled
     */
    property bool transitionEnabled: true
    /*!
      Exposes whether the orientationTransition is running.

      \qmlproperty alias rotating
     */
    readonly property bool rotating: d.transitioning

    /*!
      \qmlproperty int __orientationAngle
      \deprecated

      Use orientationAngle instead.
     */
    property alias __orientationAngle: orientationHelper.orientationAngle

    /*!
      \qmlproperty int orientationAngle

      Calculates the current orientation angle.
     */
    property int orientationAngle: automaticOrientation ? Screen.angleBetween(Screen.primaryOrientation, Screen.orientation) : 0
    // Screen.primaryOrientation and Screen.orientation can change one right after the other,
    // causing orientationAngle to momentarily change. To avoid responding to such
    // intermediate states, wait for its value to stabilize before rotating to it.
    onOrientationAngleChanged: { automaticOrientation ? stableOrientationAngleTimer.restart() : d.tryUpdateState(); }

    /*!
      The property holds if the OrientationHelper should automatically resize the
      contents when the input method appears

      The default value is false.
      */
    property bool anchorToKeyboard: false

    x: parent ? (parent.width - width) / 2 : 0
    y: parent ? (parent.height - height) / 2 : 0
    width: parent ? (d.flipDimensions ? d.availableParentHeight : parent.width) : 0
    height: parent ? (d.flipDimensions ? parent.width : d.availableParentHeight) : 0

    /*
      The attached property Screen.orientation is only valid inside Item or
      derived components. Inside Object it evaluates to 0 with no error.
      Also be aware that some apps eg. webbrowser-app set window.contentOrientation
      and thus can hide failure to update it from this code.
      See http://qt-project.org/doc/qt-5.0/qtquick/qml-qtquick-window2-screen.html
    */
    Item {
        id: d

        property real availableParentHeight: {
            if (!orientationHelper.parent)
                return 0;

            var availableHeight = orientationHelper.parent.height;
            if (d.stateAngle === 0 && UbuntuApplication.inputMethod.visible && anchorToKeyboard)
                availableHeight -= UbuntuApplication.inputMethod.keyboardRectangle.height;
            return availableHeight;
        }

        /*!
          'window' is defined by QML between startup and showing on the screen.
          There is no signal for when it becomes available and re-declaring it is not safe.

          http://qt-project.org/doc/qt-5.1/qtqml/qml-qtqml2-qt.html
          http://qt-project.org/doc/qt-5.1/qtquick/qmlmodule-qtquick-window2-qtquick-window-2.html
         */
        property bool windowActive: typeof window != 'undefined'

        /*!
          Report the current orientation of the application via QWindow::contentOrientation.
          http://qt-project.org/doc/qt-5.0/qtgui/qwindow.html#contentOrientation-prop
         */
        function applyOrientation() {
            if (windowActive && window)
                window.contentOrientation = Screen.orientation
        }

        onWindowActiveChanged: {
            if (automaticOrientation)
                applyOrientation();
        }

        Timer {
            id: stableOrientationAngleTimer
            interval: 250
            onTriggered: { d.tryUpdateState(); }
        }

        readonly property bool flipDimensions: d.stateAngle == 90 || d.stateAngle == 270

        property bool transitioning: false
        onTransitioningChanged: { d.tryUpdateState(); }

        // NB: Using a binding would cause QML to detect a
        //     binding loop and break the binding, thus the imperative code to update the state.
        function tryUpdateState() {
            if (!d.transitioning) {
                d.oldAngle = d.stateAngle;
                d.stateAngle = orientationHelper.orientationAngle;
                d.state = orientationHelper.orientationAngle.toString();
                d.applyOrientation();
            }
        }

        state: "0"
        states: [
            State { name: "0" },
            State { name: "90" },
            State { name: "180" },
            State { name: "270" }
        ]
        property int stateAngle: 0
        property int oldAngle: 0

        transitions: [
            Transition {
                id: animatedTransition
                enabled: orientationHelper.transitionEnabled
                SequentialAnimation {

                    ScriptAction { script: { d.transitioning = true; } }
                    RotationAnimator {
                        target: orientationHelper
                        duration: UbuntuAnimation.FastDuration
                        easing: UbuntuAnimation.StandardEasing
                        direction: RotationAnimation.Shortest
                        from: d.oldAngle
                        to: d.stateAngle
                    }
                    ScriptAction { script: { d.transitioning = false; } }
                }
            },
            Transition {
                enabled: !orientationHelper.transitionEnabled
                ScriptAction { script: { orientationHelper.rotation = d.stateAngle; } }
            }
        ]
    }
}
